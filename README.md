# node-subimage

Fuzzy search for subimage within image. Tolerates color drift and bad pixels.

## Installation

`npm install @ineentho/subimage`

or

`yarn add @ineentho/subimage`

## How to use

``` js
const fs = require('fs'),
      subimage = require('@ineentho/subimage')

let image = fs.createReadStream('image.png'),
    template = fs.createReadStream('template.png')

image = await subimage.util.streamParse(image)
template = await subimage.util.streamParse(template)

let results = await subimage.search(image, template)
```
## API

**search(image, template, [options])**

**util.streamParse(fileStream)**

**util.pngjsParse(pngjs)**

### image and template

Image object should have the following example structure:

``` js
{
  width: 10,
  height: 10,
  channels: 3,
  data: <Buffer ff ff ff ...>
}
```
where

- `width` Number - image width
- `height`Number - image height
- `channels` Number - optional, number of color channels in an image, possible values: 1-4
- `data` Buffer - image pixel data

Property `channels` is optional and is only used for `data` length validation.

Property `data` should be of type buffer with pixel data arranged from top-leftmost to bottom-rightmost pixel. Possible channel orders are listed bellow:

- K (grayscale)
- KA (grayscale + alpha)
- RGB
- RGBA

For example, let's say we have a 2x2 pixel image with red background and blue pixel on the bottom left corner. So the data buffer would look like this:

``` js
<Buffer ff 00 00 ff 00 00 00 00 ff ff 00 00>
```

### options

Two options are supported:

- `colorTolerance` Number - the maximum range in color difference between two matched pixels to constitute a match.
- `pixelTolerance` Number - the number of not matching (bad) pixels to ignore and treat subimage as still matching.

Options `colorTolerance` and `pixelTolerance` can be used together.

Option `colorTolerance` is combined for all color channels. For example, if `colorTolerance == 10`, then the difference for R channel can be 6, G - 4, and B should match exactly, for the pixel color to be treated as matching.

### result

The search function returns an array of result objects. If there were no matches of the subimage within the template, the result array will be empty. The result object has 3 properties: `x`, `y`, and `accuracy`. The later doesn't bear any strict meaning and is only used for ordinal comparison. The smaller the `accuracy` value, the more accurate the match between the template and the subimage is.

Example:

``` js
{ x: 2, y: 2, accuracy: 0 }
```

## Under the hood

Image pixel comparison requires a lot of steps of algebraic computation which spawns large loops of few small number operations for each step. JavaScript doesn't have native SIMD support, although there are signs of promising [initiatives](https://01.org/blogs/tlcounts/2014/bringing-simd-javascript) and the situation can change eventually. As of today, there's no other way to speed things up as to use native bindings to some algebra library that supports vectorization. Since the image data can be expressed as a matrix, [Eigen](http://eigen.tuxfamily.org/) C++ template library is used in this project.

## License

ISC
