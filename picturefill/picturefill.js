/*jshint loopfunc: true, browser: true, curly: true, eqeqeq: true, expr: true, forin: true, latedef: true, newcap: true, noarg: true, trailing: true, undef: true, unused: true */
/*! Picturefill - Author: Scott Jehl, 2012 | License: MIT/GPLv2 */
(function( w ){

  // Enable strict mode.
  "use strict";

  w.picturefill = function() {
    // Copy attributes from the source to the destination.
    function _copyAttributes(src, tar) {
      if (src.getAttribute('width') && src.getAttribute('height')) {
        tar.width = src.getAttribute('width');
        tar.height = src.getAttribute('height');
      }
    }

    // Get all picture tags.
    var ps = w.document.getElementsByTagName('span');

    // Loop the pictures.
    for (var i = 0, il = ps.length; i < il; i++ ) {
      if (ps[i].getAttribute('data-picture') !== null) {
        var sources = ps[i].getElementsByTagName('span');
        var picImg = null;
        var matches = [];

        // See which sources match.
        for (var j = 0, jl = sources.length; j < jl; j++ ) {
          var media = sources[j].getAttribute('data-media');

          // adds -webkit- prefix in javascript instead of HTML, for simplicity
          if (media && media.indexOf("device-pixel-ratio") > -1 ) {
            var webkitmedia = media.replace("device-pixel-ratio", "-webkit-device-pixel-ratio");
            var mozmedia = media.replace("device-pixel-ratio", "-moz-device-pixel-ratio");
            var omedia = media.replace("device-pixel-ratio", "-o-device-pixel-ratio");
            media = webkitmedia +","+ mozmedia +","+ omedia +","+ media;
          }
          if (media && media.indexOf("min-device-pixel-ratio") > -1 ) {
            var webkitmedia = media.replace("min-device-pixel-ratio", "-webkit-min-device-pixel-ratio");
            var mozmedia = media.replace("min-device-pixel-ratio", "min--moz-device-pixel-ratio");
            var omedia = media.replace("min-device-pixel-ratio", "-o-min-device-pixel-ratio");
            media = webkitmedia +","+ mozmedia +","+ omedia +","+ media;
          }
          if (media && media.indexOf("max-device-pixel-ratio") > -1 ) {
            var webkitmedia = media.replace("max-device-pixel-ratio", "-webkit-max-device-pixel-ratio");
            var mozmedia = media.replace("max-device-pixel-ratio", "max--moz-device-pixel-ratio");
            var omedia = media.replace("max-device-pixel-ratio", "-o-max-device-pixel-ratio");
            media = webkitmedia +","+ mozmedia +","+ omedia +","+ media;
          }

          // If there's no media specified or the media query matches, add it.
          if (!media || (w.matchMedia && w.matchMedia(media).matches)) {
            matches.push(sources[j]);
          }
        }

        if (matches.length) {
          // Grab the most appropriate (last) match.
          var match = matches.pop();
          var srcset = match.getAttribute('data-srcset');

          // Find any existing img element in the picture element.
          picImg = ps[i].getElementsByTagName('img')[0];

          // Add a new img element if one doesn't exists.
          if (!picImg) {
            picImg = w.document.createElement('img');
            picImg.alt = ps[i].getAttribute('data-alt');
            ps[i].appendChild(picImg);
          }

          // Source element uses a srcset.
          if (srcset) {
            var screenRes = w.devicePixelRatio || 1;
            // Split comma-separated `srcset` sources into an array.
            sources = srcset.split(', ');

            // Loop through each source/resolution in srcset.
            for (var res = sources.length, r = res - 1; r >= 0; r-- ) {
              // Remove any leading whitespace, then split on spaces.
              var source = sources[ r ].replace(/^\s*/, '').replace(/\s*$/, '').split(' ');
              // Parse out the resolution for each source in `srcset`.
              var resMatch = parseFloat(source[1], 10);

              if (screenRes >= resMatch) {
                if (picImg.getAttribute('src') !== source[0]) {
                  var newImg = document.createElement('img');

                  newImg.src = source[0];
                  // When the image is loaded, set a width equal to that of the
                  // original’s intrinsic width divided by the screen resolution.
                  newImg.onload = function() {
                    // Clone the original image into memory so the width is
                    // unaffected by page styles.
                    var w = this.cloneNode(true).width;
                    if (w > 0) {
                      this.width = (w / resMatch);
                    }
                  };
                  // Copy width and height from the source tag to the img element.
                  _copyAttributes(match, newImg);
                  picImg.parentNode.replaceChild(newImg, picImg);
                }
                // We’ve matched, so bail out of the loop here.
                break;
              }
            }
          } else {
            // No srcset used, so just use the 'src' value.
            picImg.src = match.getAttribute('data-src');
            // Copy width and height from the source tag to the img element.
            _copyAttributes(match, picImg);
          }
        }
      }
    }
  };

  // Run on resize and domready (w.load as a fallback)
  if (w.addEventListener) {
    w.addEventListener('resize', w.picturefill, false);
    w.addEventListener('DOMContentLoaded', function() {
      w.picturefill();
      // Run once only.
      w.removeEventListener('load', w.picturefill, false);
    }, false);
    w.addEventListener('load', w.picturefill, false);
  }
  else if (w.attachEvent) {
    w.attachEvent('onload', w.picturefill);
  }
})(this);