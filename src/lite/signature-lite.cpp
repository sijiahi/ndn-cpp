/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2015 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version, with the additional exemption that
 * compiling, linking, and/or using OpenSSL is allowed.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * A copy of the GNU Lesser General Public License is in the file COPYING.
 */

#include "../c/data.h"
#include <ndn-cpp/lite/signature-lite.hpp>

namespace ndn {

SignatureLite::SignatureLite
  (ndn_NameComponent* keyNameComponents, size_t maxKeyNameComponents)
{
  ndn_Signature_initialize(this, keyNameComponents, maxKeyNameComponents);
}

void
SignatureLite::clear() { ndn_Signature_clear(this); }

ndn_Error
SignatureLite::set(const SignatureLite& other)
{
  return ndn_Signature_setFromSignature(this, &other);
}

}
