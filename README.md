# Picture element

This module allows administrators to optimize images for different devices and
resolutions by pairing image styles with CSS media queries. It is implemented as
a display formatter for image fields.

The Picture module uses the `<picture>` element along with a polyfill library to
provide backward compatibility with older browsers.

While this module cannot account for different connection speeds (such as WiFi
vs. 3g), it does limit wasted bandwidth by ensuring delivery of only one image,
optimized for the device being used, and therefor can improve mobile
performance.

## Order of breakpoints

Since this version uses picturefill 2.0, you have to order your breakpoints from
the "largest" (e.g. wide) to the "smallest" (e.g mobile), but keep in mind that
it depends if you use min or max queries, the picture element (and the polyfill)
will use the first matching source tag (reading from top to bottom).

## Installation

- Install this module using the [official Backdrop CMS instructions](https://backdropcms.org/guide/modules)


## Dependencies

* [Breakpoints](https://github.com/backdrop-contrib/breakpoints)
* [Preset](https://github.com/backdrop-contrib/preset)

## Usage

- Usage instructions can be [viewed and edited in the Wiki](https://github.com/backdrop-contrib/picture/wiki).

## Issues

 - Bugs and Feature requests should be reported in the [Issue Queue](https://github.com/backdrop-contrib/picture/issues).

## Current Maintainers

 - [Laryn Kragt Bakker](https://github.com/laryn).
 - Collaboration and co-maintainers welcome!

## Credits

 - Current development is supported by [Aten Design Group](https://aten.io).
 - Ported to Backdrop CMS by [Laryn Kragt Bakker](https://github.com/laryn).
 - Maintained for Drupal 7 by [Peter Droogmans](https://www.drupal.org/u/attiks),
   [Jelle Sebreghts](https://www.drupal.org/u/jelle_s), and
   [Lesley Fernandes Moreira](https://www.drupal.org/u/lesleyfernandes).

 ## License

This project is GPL v2 software. See the LICENSE.txt file in this directory for
complete text.
