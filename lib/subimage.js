const searchNative = require('bindings')('search.node').search,
      util = require('./util')

module.exports.search = search
module.exports.util = util

async function search (image, template, options) {
  validateImage(image, 'image')
  validateImage(template, 'template')

  let colorTolerance = (options && options.colorTolerance) || 0,
      pixelTolerance = (options && options.pixelTolerance) || 0,
      imgMatrix = createMatrix(image),
      tplMatrix = createMatrix(template)

  return new Promise((resolve, reject) => {
    searchNative(imgMatrix, tplMatrix, colorTolerance, pixelTolerance, function (error, result) {
      result = focus(result, tplMatrix)

      result = result.map(match => ({
        x: match.col,
        y: match.row,
        accuracy: match.accuracy
      }))

      result.sort((obj1, obj2) => obj1.accuracy - obj2.accuracy)

      if (error) reject(error)
      else resolve(result)
    })
  })
}

function validateImage (image, name) {
  if (!image) {
    throw new Error('Bad ' + name + ' object')
  }
  else if (!image.data || !Buffer.isBuffer(image.data)) {
    throw new Error('Missing ' + name + ' data')
  }
  else if (isNaN(image.width) || isNaN(image.height)) {
    throw new Error('Missing ' + name + ' dimensions')
  }
  else if (isNaN(image.channels) || image.channels < 1 || image.channels > 4) {
    throw new Error('Bad number of ' + name + ' channels')
  }
  else if (image.width * image.height !== image.data.length / image.channels) {
    throw new Error('Bad ' + name + ' dimensions')
  }
}

function focus (result, tplMatrix) {
  var rows,
      cols,
      out,
      prevl,
      l,
      skip,
      ovps

  rows = tplMatrix.rows
  cols = tplMatrix.cols

  if (result.length === 1) {
    return result
  }

  if (!result.length) {
    return []
  }

  out = result.reduce(function (prev, curr, i) {
    if (i === 1) {
      prev = [ prev ]
    }

    skip = false
    l = prev.length
    while (l--) {
      prevl = prev[l]
      ovps = overlaps(prevl, curr, rows, cols)
      skip = skip || ovps

      if (ovps && curr.accuracy < prevl.accuracy) {
        prev[l] = curr
      }
    }

    if (!skip) {
      prev.push(curr)
    }

    return prev
  })

  out = out.reduce(function (prev, curr, i) {
    if (i === 1) {
      prev = [ prev ]
    }

    l = prev.length
    skip = false
    while (l--) {
      prevl = prev[l]
      if (prevl.row === curr.row &&
                prevl.col === curr.col &&
                prevl.accuracy === curr.accuracy) {
        skip = true
        break
      }
    }

    if (!skip) {
      prev.push(curr)
    }

    return prev
  })

  return out
}

function overlaps (a, b, r, c) {
  var ac = a.col,
      ar = a.row,
      bc = b.col,
      br = b.row,

      bw = c + bc,
      bh = r + br,
      aw = c + ac,
      ah = r + ar

  return ((bw < bc || bw > ac) &&
            (bh < br || bh > ar) &&
            (aw < ac || aw > bc) &&
            (ah < ar || ah > br))
}

function createMatrix (image) {
  var channels,
      length,
      out

  channels = image.channels
  length = image.data.length / channels

  out = {
    rows: image.height,
    cols: length / image.height,
    channels: channels
  }

  out.data = new Array(channels)
  var l = out.data.length
  while (l--) {
    out.data[l] = new Float32Array(length)
  }

  split(image.data, out.data)

  return out
}

function split (raw, out) {
  let channels = out.length
  for (let i = 0, k = 0, l = raw.length; i < l; k++, i += channels) {
    for (let j = 0; j < channels; j++) {
      out[j][k] = raw[i + j]
    }
  }
}
