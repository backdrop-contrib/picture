if (Drupal && jQuery) {
  // only load if Drupal and jQuery are defined.
  (function ($) {
    Drupal.behaviors.picture = {
      attach: function (context) {
        if (context[0]) {
          window.picturefill(context[0]);
        }
      }
    };
  })(jQuery);
}