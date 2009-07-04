// $Id: lockdown.admin.js,v 1.1 2009/06/29 02:24:31 arto Exp $

if (Drupal.jsEnabled) {
  $(document).ready(function() {
    // Overrides
    var toggleOverride = function(checkbox) {
      $("#" + checkbox.id.replace('-override', '-variables')).css("display", checkbox.checked ? "block" : "none");
    };
    $(".form-checkbox.lockdown-override").each  (function() { toggleOverride(this); });
    $(".form-checkbox.lockdown-override").change(function() { toggleOverride(this); });

    // IP address retention policy
    $("#edit-config-retention-variables input[@type='radio']").change(function() {
      $("#edit-config-retention-ip-table").css("display", this.value == '1' ? "block" : "none");
    });
  });
}
