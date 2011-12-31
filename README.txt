Campaign Origins module allows the website to accept codes from marketing 
campaigns VIA the URL and piggybacks them based on the specified days to 
store them in an administration settings form. A function is provided to 
store this information when a conversion has occurred such as contact form, 
email signup, donation form, etc. This module needs cookies to function.

 * Per-field upload control (file extensions, file size).
 * Per-node upload size limits.
 * Multiple fields per content type.
 * Customizable paths for saving uploads (plus token support for dynamic paths).
 * Icons for uploaded file types.

Campaign Origins was written by Richard Castera (http://drupal.org/node/830660).

Install
-------

1) Copy the campaign_origins folder to the modules folder in your installation.

2) Enable the module using Administer -> Site building -> Modules (/admin/build/modules).

3) Create a new file field in through CCK's interface. Visit Administer ->
   Content management -> Content types (admin/content/types), then click
   Manage fields on the type you want to add an file upload field. Select
   "File" as the field type and "File" as the widget type to create a new
   field.

4) Upload files on the node form for the type that you set up.


--------

just hook into the form that you want to document the conversion and call this function to document a conversion
campaign_origins_save_origin('donation_form', $id);
