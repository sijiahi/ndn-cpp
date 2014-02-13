/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2014 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_META_INFO_HPP
#define NDN_META_INFO_HPP

#include <math.h>
#include "name.hpp"
#include "c/data-types.h"

struct ndn_MetaInfo;

namespace ndn {

/**
 * A MetaInfo holds the meta info which is signed inside the data packet.
 */
class MetaInfo {
public:
  MetaInfo() 
  : changeCount_(0)
  {   
    timestampMilliseconds_ = -1;
    type_ = ndn_ContentType_BLOB;
    freshnessPeriod_ = -1;
  }

  /**
   * Set the metaInfoStruct to point to the values in this meta info object, without copying any memory.
   * WARNING: The resulting pointers in metaInfoStruct are invalid after a further use of this object which could reallocate memory.
   * @param metaInfoStruct a C ndn_MetaInfo struct where the name components array is already allocated.
   */
  void 
  get(struct ndn_MetaInfo& metaInfoStruct) const;

  /**
   * Clear this meta info, and set the values by copying from the ndn_MetaInfo struct.
   * @param metaInfoStruct a C ndn_MetaInfo struct
   */
  void 
  set(const struct ndn_MetaInfo& metaInfoStruct);

  MillisecondsSince1970 
  getTimestampMilliseconds() const { return timestampMilliseconds_; }
  
  ndn_ContentType 
  getType() const { return type_; }
  
  Milliseconds 
  getFreshnessPeriod() const { return freshnessPeriod_; }

  /**
   * @deprecated Use getFreshnessPeriod.
   */
  int 
  getFreshnessSeconds() const { return freshnessPeriod_ < 0 ? -1 : (int)round(freshnessPeriod_ / 1000.0); }
  
  const Name::Component& 
  getFinalBlockID() const { return finalBlockID_; }
  
  void 
  setTimestampMilliseconds(MillisecondsSince1970 timestampMilliseconds) 
  { 
    timestampMilliseconds_ = timestampMilliseconds; 
    ++changeCount_;
  }
  
  void 
  setType(ndn_ContentType type) 
  { 
    type_ = type; 
    ++changeCount_;
  }

  void 
  setFreshnessPeriod(Milliseconds freshnessPeriod) 
  { 
    freshnessPeriod_ = freshnessPeriod; 
    ++changeCount_;
  }
  
  /**
   * @deprecated Use setFreshnessPeriod.
   */
  void 
  setFreshnessSeconds(int freshnessSeconds) { setFreshnessPeriod(freshnessSeconds < 0 ? -1.0 : (double)freshnessSeconds * 1000.0); }

  void 
  setFinalBlockID(const Name::Component& finalBlockID) 
  { 
    finalBlockID_ = finalBlockID; 
    ++changeCount_;
  }
    
  /**
   * Get the change count, which is incremented each time this object is changed.
   * @return The change count.
   */
  uint64_t 
  getChangeCount() const { return changeCount_; }

private:
  MillisecondsSince1970 timestampMilliseconds_; /**< milliseconds since 1/1/1970. -1 for none */
  ndn_ContentType type_;         /**< default is ndn_ContentType_BLOB. -1 for none */
  Milliseconds freshnessPeriod_; /**< -1 for none */
  Name::Component finalBlockID_; /** size 0 for none */
  uint64_t changeCount_;
};

}

#endif