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

#ifndef NDN_INTEREST_LITE_HPP
#define NDN_INTEREST_LITE_HPP

#include "exclude-lite.hpp"
#include "key-locator-lite.hpp"
#include "../c/interest-types.h"

namespace ndn {

/**
 * An InterestLite holds a NameLite and other fields for an interest.
 */
class InterestLite : private ndn_Interest {
public:
  /**
   * Create an InterestLite with the pre-allocated nameComponents and
   * excludeEntries, and defaults for all the values.
   * @param nameComponents The pre-allocated array of ndn_NameComponent. Instead
   * of an array of NameLite::Component, this is an array of the underlying
   * ndn_NameComponent struct so that it doesn't run the default constructor
   * unnecessarily.
   * @param maxNameComponents The number of elements in the allocated
   * nameComponents array.
   * @param excludeEntries The pre-allocated array of ndn_ExcludeEntry. Instead
   * of an array of ExcludeLite::Entry, this is an array of the underlying
   * ndn_ExcludeEntry struct so that it doesn't run the default constructor
   * unnecessarily.
   * @param maxExcludeEntries The number of elements in the allocated
   * excludeEntries array.
   * @param keyNameComponents The pre-allocated array of ndn_NameComponent for
   * the keyLocator. Instead of an array of NameLite::Component, this is an
   * array of the underlying ndn_NameComponent struct so that it doesn't run the
   * default constructor unnecessarily.
   * @param maxKeyNameComponents The number of elements in the allocated
   * keyNameComponents array.
   */
  InterestLite
    (ndn_NameComponent *nameComponents, size_t maxNameComponents,
     ndn_ExcludeEntry *excludeEntries, size_t maxExcludeEntries,
     ndn_NameComponent *keyNameComponents, size_t maxKeyNameComponents);

  NameLite&
  getName() { return NameLite::upCast(name); }

  const NameLite&
  getName() const { return NameLite::upCast(name); }

  int
  getMinSuffixComponents() const { return minSuffixComponents; }

  int
  getMaxSuffixComponents() const { return maxSuffixComponents; }

  const KeyLocatorLite&
  getKeyLocator() const { return KeyLocatorLite::upCast(keyLocator); }

  KeyLocatorLite&
  getKeyLocator() { return KeyLocatorLite::upCast(keyLocator); }

  ExcludeLite&
  getExclude() { return ExcludeLite::upCast(exclude); }

  const ExcludeLite&
  getExclude() const { return ExcludeLite::upCast(exclude); }

  int
  getChildSelector() const { return childSelector; }

  /**
   * Return true if the content must be fresh. The default is true.
   * @return true if must be fresh, otherwise false.
   */
  bool
  getMustBeFresh() const;

  int
  getScope() const { return scope; }

  ndn_Milliseconds
  getInterestLifetimeMilliseconds() const { return interestLifetimeMilliseconds; }

  const BlobLite&
  getNonce() const { return BlobLite::upCast(nonce); }

  /**
   * Set the interest name.
   * @param name The interest name. This only copies the pointer to the name
   * components array, but does not copy the component values.
   * @return This InterestLite so that you can chain calls to update values.
   */
  InterestLite&
  setName(const NameLite& name)
  {
    NameLite::upCast(this->name) = name;
    return *this;
  }

  /**
   * Set the min suffix components count.
   * @param minSuffixComponents The min suffix components count. If not
   * specified, set to -1.
   * @return This InterestLite so that you can chain calls to update values.
   */
  InterestLite&
  setMinSuffixComponents(int minSuffixComponents)
  {
    this->minSuffixComponents = minSuffixComponents;
    return *this;
  }

  /**
   * Set the max suffix components count.
   * @param maxSuffixComponents The max suffix components count. If not
   * specified, set to -1.
   * @return This InterestLite so that you can chain calls to update values.
   */
  InterestLite&
  setMaxSuffixComponents(int maxSuffixComponents)
  {
    this->maxSuffixComponents = maxSuffixComponents;
    return *this;
  }

  /**
   * Set the child selector.
   * @param childSelector The child selector. If not specified, set to -1.
   * @return This InterestLite so that you can chain calls to update values.
   */
  InterestLite&
  setChildSelector(int childSelector)
  {
    this->childSelector = childSelector;
    return *this;
  }

  /**
   * Set the MustBeFresh flag.
   * @param mustBeFresh True if the content must be fresh, otherwise false. If
   * you do not set this flag, the default value is true.
   * @return This InterestLite so that you can chain calls to update values.
   */
  InterestLite&
  setMustBeFresh(bool mustBeFresh);

  /**
   * Set the interest scope.
   * @param scope The interest scope. If not specified, set to -1.
   * @return This InterestLite so that you can chain calls to update values.
   */
  InterestLite&
  setScope(int scope)
  {
    this->scope = scope;
    return *this;
  }

  /**
   * Set the interest lifetime.
   * @param interestLifetimeMilliseconds The interest lifetime in milliseconds.
   * If not specified, set to -1.
   * @return This InterestLite so that you can chain calls to update values.
   */
  InterestLite&
  setInterestLifetimeMilliseconds(ndn_Milliseconds interestLifetimeMilliseconds)
  {
    this->interestLifetimeMilliseconds = interestLifetimeMilliseconds;
    return *this;
  }
};

}

#endif
