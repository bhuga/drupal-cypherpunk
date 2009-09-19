// $Id: README.txt,v 1.2 2009/09/17 09:39:08 arto Exp $

OpenPGP for Drupal
==================
OpenPGP is the most widely-used e-mail encryption standard in the world.
Originally derived from PGP (Pretty Good Privacy), first created by Phil
Zimmermann in 1991, the standard is now defined by the IETF in RFC 4880.

This module provides encryption for Drupal's outgoing e-mails using GNU
Privacy Guard (GnuPG), an open-source implementation of the OpenPGP
standard.

  <http://drupal.org/project/openpgp>

For more information about OpenPGP, see:

  <http://www.openpgp.org/>
  <http://tools.ietf.org/html/rfc4880>

For more information about the GNU Privacy Guard, please refer to:

  <http://www.gnupg.org/>


FEATURES FOR SITE BUILDERS
--------------------------
* Encrypts Drupal's outgoing e-mails to the public key of the recipient(s),
  when available.


FEATURES FOR ADMINISTRATORS
---------------------------
* Includes an administration screen at [Administer >> User management >>
  Public keys] for displaying and managing users' public keys on file.
* Provides configuration for the location of the `gpg' command-line binary
  and the GnuPG home directory.
* Provides system status report entries indicating whether GnuPG is
  installed correctly and helping to troubleshoot installation problems.
* Includes support for site-specific plaintext headers, footers and ASCII
  armor comments in outgoing encrypted e-mails.


FEATURES FOR DEVELOPERS
-----------------------
* Maintains a secure site-specific GnuPG keyring (.gnupg/pubring.gpg).
* Implements a simple and easy-to-use asymmetric encryption/decryption API.


BUG REPORTS
-----------
Post bug reports and feature requests to the issue tracking system at:

  <http://drupal.org/node/add/project-issue/openpgp>


FEATURE REQUESTS
----------------
The author is available for contract development and customization relating
to this module. You can reach him at <http://drupal.org/user/26089/contact>.


CREDITS
-------
Developed and maintained by Arto Bendiken <http://ar.to/>
Developed for Cypherpunk Drupal <http://drupal.org/project/cypherpunk>
