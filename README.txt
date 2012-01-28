Campaign Origins is a Drupal 6 module that allows a website to accept 
codes from marketing campaigns VIA the URL and stores them in a cookie 
for a number of days set in the administration settings form. A function 
is provided to store this information when a conversion has occurred 
such as contact form, email signup, donation form, etc. This module 
needs cookies to function.

 * Set the number of days a cookie expires.
 * Add unlimited amount of origin codes with descriptions.
 * Keep track and report on a full inventory of origin codes.
 * Full reporting with filterable and sortable results.

Install
-----------------------

1) Copy the campaign_origins folder to the modules folder in your
   installation.

2) Enable the module using Administer -> Site building -> Modules 
   (/admin/build/modules).

3) Go to (admin/settings/campaign-origins) to set the number of 
   days the cookie should expire.

4) Go here (settings/campaign-origins/add-campaign-origins) to add 
   a new origin code ('facebook' for example).

5) To allow your website to accept origin codes, setup your link 
   structure as such http://example.com/signup?origin=facebook.

6) Just hook into the form that you want to document the conversion 
   and call this function to document a conversion 
   campaign_origins_save_origin('donation_form', $id);
