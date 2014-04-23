/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <algorithm>
#include <fstream>
#include "../../c/util/crypto.h"
#include "../../encoding/base64.hpp"
#include <ndn-cpp/security/security-exception.hpp>
#include <ndn-cpp/security/identity/file-private-key-storage.hpp>

using namespace std;

namespace ndn {

static const char *WHITESPACE_CHARS = " \n\r\t";

/**
 * Modify str in place to erase whitespace on the left.
 * @param str
 */
static inline void 
trimLeft(string& str)
{
  size_t found = str.find_first_not_of(WHITESPACE_CHARS);
  if (found != string::npos) {
    if (found > 0)
      str.erase(0, found);
  }
  else
    // All whitespace
    str.clear();    
}

/**
 * Modify str in place to erase whitespace on the right.
 * @param str
 */
static inline void 
trimRight(string& str)
{
  size_t found = str.find_last_not_of(WHITESPACE_CHARS);
  if (found != string::npos) {
    if (found + 1 < str.size())
      str.erase(found + 1);
  }
  else
    // All whitespace
    str.clear();
}

/**
 * Modify str in place to erase whitespace on the left and right.
 * @param str
 */
static void 
trim(string& str)
{
  trimLeft(str);
  trimRight(str);
}

FilePrivateKeyStorage::FilePrivateKeyStorage()
{
  // Note: We don't use <filesystem> support because it is not "header-only" and 
  // require linking to libraries.
  // TODO: Handle non-unix file system paths which don't use '/'.
  const char* home = getenv("HOME");
  if (!home || *home == '\0')
    // Don't expect this to happen;
    home = ".";
  string homeDir(home);
  if (homeDir[homeDir.size() - 1] == '/')
    // Strip the ending '/'.
    homeDir.erase(homeDir.size() - 1);
  
  keystorePath_ = homeDir + '/' + ".ndn/ndnsec-tpm-file";
  // TODO: Handle non-unix file systems which don't have "mkdir -p".
  ::system(("mkdir -p " + keystorePath_).c_str());
}

FilePrivateKeyStorage::~FilePrivateKeyStorage()
{
}

void 
FilePrivateKeyStorage::generateKeyPair
  (const Name& keyName, KeyType keyType, int keySize)
{
#if 1
  throw runtime_error("FilePrivateKeyStorage::generateKeyPair not implemented");
#endif
}

ptr_lib::shared_ptr<PublicKey> 
FilePrivateKeyStorage::getPublicKey(const Name& keyName)
{
  string keyURI = keyName.toUri();

  if (!doesKeyExist(keyName, KEY_CLASS_PUBLIC))
    throw SecurityException("Public Key does not exist.");

  ifstream file(nameTransform(keyURI, ".pub").c_str());
  stringstream base64;
  base64 << file.rdbuf();
  
  // Use a vector in a shared_ptr so we can make it a Blob without copying.
  ptr_lib::shared_ptr<vector<uint8_t> > der(new vector<uint8_t>());
  fromBase64(base64.str(), *der);
  return PublicKey::fromDer(Blob(der));
}

Blob 
FilePrivateKeyStorage::sign
  (const uint8_t *data, size_t dataLength, const Name& keyName, 
   DigestAlgorithm digestAlgorithm)
{
  string keyURI = keyName.toUri();

  if (!doesKeyExist(keyName, KEY_CLASS_PRIVATE))
    throw SecurityException
      ("FilePrivateKeyStorage::sign: private key doesn't exist");

  if (digestAlgorithm != DIGEST_ALGORITHM_SHA256)
    throw SecurityException
      ("FilePrivateKeyStorage::sign: Unsupported digest algorithm");
  
  // Read the private key.
  ifstream file(nameTransform(keyURI, ".pri").c_str());
  stringstream base64;
  base64 << file.rdbuf();
  vector<uint8_t> der;
  fromBase64(base64.str(), der);
  // Use a temporary pointer since d2i updates it.
  const uint8_t* derPointer = (const uint8_t*)&der[0];
  rsa_st* privateKey = d2i_RSAPrivateKey(NULL, &derPointer, der.size());
  if (!privateKey)
    throw SecurityException
      ("FilePrivateKeyStorage::sign: Error decoding private key DER");

  // Sign.
  uint8_t digest[SHA256_DIGEST_LENGTH];
  ndn_digestSha256(data, dataLength, digest);
  // TODO: use RSA_size to get the proper size of the signature buffer.
  uint8_t signatureBits[1000];
  unsigned int signatureBitsLength;
  int success = RSA_sign
    (NID_sha256, digest, sizeof(digest), signatureBits, &signatureBitsLength, 
     privateKey);
  // Free the private key before checking for success.
  RSA_free(privateKey);
  if (!success)
    throw SecurityException("FilePrivateKeyStorage::sign: Error in RSA_sign");
  
  return Blob(signatureBits, (size_t)signatureBitsLength);
}

Blob 
FilePrivateKeyStorage::decrypt
  (const Name& keyName, const uint8_t* data, size_t dataLength, 
   bool isSymmetric)
{
#if 1
  throw runtime_error("FilePrivateKeyStorage::decrypt not implemented");
#endif
}

Blob
FilePrivateKeyStorage::encrypt
  (const Name& keyName, const uint8_t* data, size_t dataLength, 
   bool isSymmetric)
{
#if 1
  throw runtime_error("FilePrivateKeyStorage::encrypt not implemented");
#endif
}

void 
FilePrivateKeyStorage::generateKey
  (const Name& keyName, KeyType keyType, int keySize)
{
#if 1
  throw runtime_error("FilePrivateKeyStorage::generateKey not implemented");
#endif
}

bool
FilePrivateKeyStorage::doesKeyExist(const Name& keyName, KeyClass keyClass)
{
  string keyURI = keyName.toUri();
  if (keyClass == KEY_CLASS_PUBLIC)
    return ::access(nameTransform(keyURI, ".pub").c_str(), R_OK) == 0;
  else if (keyClass == KEY_CLASS_PRIVATE)
    return ::access(nameTransform(keyURI, ".pri").c_str(), R_OK) == 0;
  else if (keyClass == KEY_CLASS_SYMMETRIC)
    return ::access(nameTransform(keyURI, ".key").c_str(), R_OK) == 0;
  else
    return false;
}

string
FilePrivateKeyStorage::nameTransform
  (const string& keyName, const string& extension)
{
  uint8_t hash[SHA256_DIGEST_LENGTH];
  ndn_digestSha256((uint8_t*)&keyName[0], keyName.size(), hash);
  
  string digest = toBase64(hash, sizeof(hash));
  trim(digest);
  std::replace(digest.begin(), digest.end(), '/', '%');

  return keystorePath_ + "/" + digest + extension;
}

}
