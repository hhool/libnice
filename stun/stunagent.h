/*
 * This file is part of the Nice GLib ICE library.
 *
 * (C) 2008 Collabora Ltd.
 * (C) 2008 Nokia Corporation. All rights reserved.
 *  Contact: Youness Alaoui
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is the Nice GLib ICE library.
 *
 * The Initial Developers of the Original Code are Collabora Ltd and Nokia
 * Corporation. All Rights Reserved.
 *
 *
 * Alternatively, the contents of this file may be used under the terms of the
 * the GNU Lesser General Public License Version 2.1 (the "LGPL"), in which
 * case the provisions of LGPL are applicable instead of those above. If you
 * wish to allow use of your version of this file only under the terms of the
 * LGPL and not to allow others to use your version of this file under the
 * MPL, indicate your decision by deleting the provisions above and replace
 * them with the notice and other provisions required by the LGPL. If you do
 * not delete the provisions above, a recipient may use your version of this
 * file under either the MPL or the LGPL.
 */

#ifndef _STUN_AGENT_H
#define _STUN_AGENT_H


#ifdef _WIN32
#include "win32_common.h"
#else
#include <stdint.h>
#include <stdbool.h>
#endif


#include <sys/types.h>

typedef struct stun_agent_t StunAgent;

#include "stunmessage.h"

typedef enum {
  STUN_COMPATIBILITY_RFC3489,
  STUN_COMPATIBILITY_RFC5389,
  STUN_COMPATIBILITY_LAST = STUN_COMPATIBILITY_RFC5389
} StunCompatibility;


typedef enum {
  /* The message is validated */
  STUN_VALIDATION_SUCCESS,
  /* This is not a valid STUN message */
  STUN_VALIDATION_NOT_STUN,
  /* The message seems to be valid but incomplete */
  STUN_VALIDATION_INCOMPLETE_STUN,
  /* The message does not have the cookie or the fingerprint
   * while the agent needs it with its usage */
  STUN_VALIDATION_BAD_REQUEST,
  /* The message is valid but unauthorized with no username and message-integrity
     attributes. A BAD_REQUEST error must be generated */
  STUN_VALIDATION_UNAUTHORIZED_BAD_REQUEST,
  /* The message is valid but unauthorized as the username/password do not match.
     An UNAUTHORIZED error must be generated */
  STUN_VALIDATION_UNAUTHORIZED,
  /* The message is valid but this is a response/error that doesn't match
   * a previously sent request */
  STUN_VALIDATION_UNMATCHED_RESPONSE,
  /* The message is valid but contains one or more unknown comprehension
   * attributes. stun_agent_build_unknown_attributes_error should be called */
  STUN_VALIDATION_UNKNOWN_REQUEST_ATTRIBUTE,
  /* The message is valid but contains one or more unknown comprehension
   * attributes. This is a response, or error, or indication message
   * and no error response should be sent */
  STUN_VALIDATION_UNKNOWN_ATTRIBUTE,
} StunValidationStatus;


#define STUN_AGENT_USAGE_SHORT_TERM_CREDENTIALS 	0x0001
#define STUN_AGENT_USAGE_LONG_TERM_CREDENTIALS 		0x0002
#define STUN_AGENT_USAGE_USE_FINGERPRINT 		0x0004
#define STUN_AGENT_USAGE_ADD_SOFTWARE 			0x0008
#define STUN_AGENT_USAGE_IGNORE_CREDENTIALS		0x0010
#define STUN_AGENT_USAGE_NO_INDICATION_AUTH		0x0020
#define STUN_AGENT_USAGE_FORCE_VALIDATER		0x0040


typedef struct {
  stun_transid_t id;
  stun_method_t method;
  uint8_t *key;
  size_t key_len;
  uint8_t long_term_key[16];
  bool long_term_valid;
  bool valid;
} StunAgentSavedIds;

struct stun_agent_t {
  StunCompatibility compatibility;
  StunAgentSavedIds sent_ids[STUN_AGENT_MAX_SAVED_IDS];
  uint16_t *known_attributes;
  uint32_t usage_flags;
};

typedef struct {
  uint8_t *username;
  size_t username_len;
  uint8_t *password;
  size_t password_len;
} stun_validater_data;


typedef bool (*StunMessageIntegrityValidate) (StunAgent *agent,
    StunMessage *message, uint8_t *username, uint16_t username_len,
    uint8_t **password, size_t *password_len, void *user_data);

bool stun_agent_default_validater (StunAgent *agent,
    StunMessage *message, uint8_t *username, uint16_t username_len,
    uint8_t **password, size_t *password_len, void *user_data);

void stun_agent_init (StunAgent *agent, const uint16_t *known_attributes,
    StunCompatibility compatibility, uint32_t usage_flags);
StunValidationStatus stun_agent_validate (StunAgent *agent, StunMessage *msg,
    const uint8_t *buffer, size_t buffer_len,
    StunMessageIntegrityValidate validater, void * validater_data);
bool stun_agent_init_request (StunAgent *agent, StunMessage *msg,
    uint8_t *buffer, size_t buffer_len, stun_method_t m);
bool stun_agent_init_indication (StunAgent *agent, StunMessage *msg,
    uint8_t *buffer, size_t buffer_len, stun_method_t m);
bool stun_agent_init_response (StunAgent *agent, StunMessage *msg,
    uint8_t *buffer, size_t buffer_len, const StunMessage *request);
bool stun_agent_init_error (StunAgent *agent, StunMessage *msg,
    uint8_t *buffer, size_t buffer_len, const StunMessage *request,
    stun_error_t err);
size_t stun_agent_build_unknown_attributes_error (StunAgent *agent,
    StunMessage *msg, uint8_t *buffer, size_t buffer_len,
    const StunMessage *request);
size_t stun_agent_finish_message (StunAgent *agent, StunMessage *msg,
   const uint8_t *key, size_t key_len);

#endif /* _STUN_AGENT_H */
