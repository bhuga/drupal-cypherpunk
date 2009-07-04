// $Id: README.txt,v 1.4 2009/06/26 17:11:29 arto Exp $

Lockdown for Drupal
===================
This module provides tools for further locking down the security of your
Drupal site. It is meant primarily for "paranoid" Drupal use cases where it
is imperative to maintain tight information discipline, particularly with
regards to search engine indexing and data retention of personally
identifying information.

  <http://drupal.org/project/lockdown>


CURRENT FEATURES
----------------
* Each lockdown level provides Drupal configuration overrides:
  * Determines if the site is online or offline.
  * Enforces configurable transport-level security policies.
  * Enforces configurable IP address retention policies.
* Integrates with Secure Pages <http://drupal.org/project/securepages>
* Integrates with Messaging Framework <http://drupal.org/project/messaging>
  * Notifies the administrator if the Lockdown module is disabled.
  * Notifies the administrator when the lockdown level is changed.
* Integrates with Rules <http://drupal.org/project/rules>
  * Provides the following Rules triggers/events:
    * "[When] HTTP request initiated"
    * "[When] HTTP request completed"
    * "[When] Lockdown level activated"
    * "[When] Lockdown level deactivated"
  * Provides the following Rules conditions:
    * "Lockdown level is [level]"
    * "Referrer contains [text]"
    * "Referrer is unspecified"
    * "Request method is [GET/POST/PUT/DELETE]"
    * "Request URI contains [text]"
    * "Visitor accepts a given language [code]"
    * "Visitor's HTTP user agent contains [text]"
    * "Visitor is a web crawler"
    * "Visitor's IP address contains [text]"
  * Provides the following Rules actions:
    * "Activate a lockdown level"
    * "Delay the HTTP response"
    * "Send a HTTP response status"
    * "Send a HTTP content type header"
    * "Terminate the HTTP connection"
    * "Terminate the HTTP session"
    * "Execute shell command"
    * "Execute SQL query"
    * "Execute PHP code"
    * "Erase system events"
    * "Erase administrator's e-mail address"
    * "Erase users' e-mail addresses"
    * "Erase the Drupal database" (use with the utmost care!)
* Provides additional user-specific privacy options:
  * Hides the user account age from anonymous visitors and search engines.
  * Hides profile pictures from anonymous visitors and search engines.
* Generates watchdog messages if Lockdown is disabled or uninstalled.


BUG REPORTS
-----------
Please post any bug reports to the drupal.org issue tracking system at:

  <http://drupal.org/node/add/project-issue/lockdown>


FEATURE REQUESTS
----------------
The author is available for contract development and customization relating
to this module. You can reach him at <http://drupal.org/user/26089/contact>.


CREDITS
-------
Developed and maintained by Arto Bendiken <http://ar.to/>
Contains code from IP Anonymize <http://drupal.org/project/ip_anon>
Contains code from Paranoia <http://drupal.org/project/paranoia>
