const stream = require('stream'),
      PNG = require('pngjs').PNG

var functions = {}

functions.pngjsParse = function (png) {
  return {
    width: png.width,
    height: png.height,
    channels: png.data.length / (png.width * png.height),
    data: png.data
  }
}

functions.streamParse = async function (str) {
  if (!(str instanceof stream)) throw new Error('invalid stream')

  return functions.pngjsParse(await new Promise((resolve, reject) => {
    let png = new PNG()
    str.pipe(png)
      .on('parsed', () => resolve(png))
      .on('error', reject)
  }))
}

module.exports = functions
