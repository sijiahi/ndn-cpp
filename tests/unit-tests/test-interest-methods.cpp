/**
 * Copyright (C) 2014 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * From PyNDN unit-tests by Adelola Bannis.
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
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * A copy of the GNU General Public License is in the file COPYING.
 */

#include "gtest/gtest.h"
#include <ndn-cpp/interest.hpp>

using namespace std;
using namespace ndn;

static const uint8_t codedInterest[] = {
0x05, 0x53, // Interest
  0x07, 0x0A, 0x08, 0x03, 0x6E, 0x64, 0x6E, 0x08, 0x03, 0x61, 0x62, 0x63, // Name
  0x09, 0x38, // Selectors
    0x0D, 0x01, 0x04, // MinSuffixComponents
    0x0E, 0x01, 0x06, // MaxSuffixComponents
    0x0F, 0x22, // KeyLocator
      0x1D, 0x20, // KeyLocatorDigest
                  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
    0x10, 0x07, // Exclude
      0x08, 0x03, 0x61, 0x62, 0x63, // NameComponent
      0x13, 0x00, // Any
    0x11, 0x01, 0x01, // ChildSelector
    0x12, 0x00, // MustBeFesh
  0x0A, 0x04, 0x61, 0x62, 0x61, 0x62,   // Nonce
  0x0B, 0x01, 0x02, // Scope
  0x0C, 0x02, 0x75, 0x30, // InterestLifetime
1
};

static string dump(const string& s1) { return s1; }
static string dump(const string& s1, const string& s2) { return s1 + " " + s2; }

static const char* initialDumpValues[] = {
  "name: /ndn/abc",
  "minSuffixComponents: 4",
  "maxSuffixComponents: 6",
  "keyLocator: KeyLocatorDigest: 000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f",
  "exclude: abc,*",
  "childSelector: 1",
  "mustBeFresh: True",
  "nonce: 61626162",
  "scope: 2",
  "lifetimeMilliseconds: 30000"
};

static string
toString(int value)
{
  ostringstream buffer;
  buffer << value;
  return buffer.str();
}

static string
toString(double value)
{
  ostringstream buffer;
  buffer << value;
  return buffer.str();
}

static vector<string>
dumpInterest(const Interest& interest)
{
  vector<string> result;

  result.push_back(dump("name:", interest.getName().toUri()));
  result.push_back(dump("minSuffixComponents:",
    interest.getMinSuffixComponents() >= 0 ?
      toString(interest.getMinSuffixComponents()) : "<none>"));
  result.push_back(dump("maxSuffixComponents:",
    interest.getMaxSuffixComponents() >= 0 ?
      toString(interest.getMaxSuffixComponents()) : "<none>"));
  if ((int)interest.getKeyLocator().getType() >= 0) {
    if (interest.getKeyLocator().getType() == ndn_KeyLocatorType_KEY_LOCATOR_DIGEST)
      result.push_back(dump("keyLocator: KeyLocatorDigest:",
        interest.getKeyLocator().getKeyData().toHex()));
    else if (interest.getKeyLocator().getType() == ndn_KeyLocatorType_KEYNAME)
      result.push_back(dump("keyLocator: KeyName:",
        interest.getKeyLocator().getKeyName().toUri()));
    else
      result.push_back(dump("keyLocator: <unrecognized KeyLocatorType"));
  }
  else
    result.push_back(dump("keyLocator: <none>"));
  result.push_back(dump("exclude:",
    interest.getExclude().size() > 0 ? interest.getExclude().toUri() :
      "<none>"));
  result.push_back(dump("childSelector:",
    interest.getChildSelector() >= 0 ? toString(interest.getChildSelector()) :
      "<none>"));
  result.push_back(dump("mustBeFresh:", 
    interest.getMustBeFresh() ? "True" : "False"));
  result.push_back(dump("nonce:", 
    interest.getNonce().size() == 0 ? "<none>" :
      interest.getNonce().toHex()));
  result.push_back(dump("scope:", 
    interest.getScope() < 0 ? "<none>" : toString(interest.getScope())));
  result.push_back(dump("lifetimeMilliseconds:",
    interest.getInterestLifetimeMilliseconds() < 0 ? "<none>" :
      toString(interest.getInterestLifetimeMilliseconds())));
                        
  return result;
};

/**
 * Return a copy of the strings array, removing any string that start with prefix.
 */
static vector<string>
removeStartingWith(const vector<string>& strings, const string& prefix)
{
  vector<string> result;
  for (size_t i = 0; i < strings.size(); ++i) {
    if (strings[i].substr(0, prefix.size()) != prefix)
      result.push_back(strings[i]);
  }

  return result;
}

// ignoring nonce, see if two data dumps are equal
static bool
interestDumpsEqual(const vector<string>& d1, const vector<string>& d2)
{
  string prefix("nonce:");
  return removeStartingWith(d1, prefix) == removeStartingWith(d2, prefix);
}

static Interest
createFreshInterest()
{
  Interest freshInterest(Name("/ndn/abc"));
  freshInterest.setMustBeFresh(false);
  freshInterest.setMinSuffixComponents(4);
  freshInterest.setMaxSuffixComponents(6);
  freshInterest.getKeyLocator().setType(ndn_KeyLocatorType_KEY_LOCATOR_DIGEST);
  uint8_t digest[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F };
  freshInterest.getKeyLocator().setKeyData(Blob(digest, sizeof(digest)));
  freshInterest.getExclude().appendComponent(Name("abc")[0]).appendAny();
  freshInterest.setInterestLifetimeMilliseconds(30000);
  freshInterest.setChildSelector(1);
  freshInterest.setMustBeFresh(true);
  freshInterest.setScope(2);

  return freshInterest;
}

class TestInterestDump : public ::testing::Test {
public:
  TestInterestDump()
  : initialDump(initialDumpValues,
      initialDumpValues + sizeof(initialDumpValues) / sizeof(initialDumpValues[0]))
  {
    referenceInterest.wireDecode(codedInterest, sizeof(codedInterest));
  }

  vector<string> initialDump;
  Interest referenceInterest;
};

TEST_F(TestInterestDump, Dump)
{
  // see if the dump format is the same as we expect
  vector<string> decodedDump = dumpInterest(referenceInterest);
  ASSERT_EQ(initialDump, decodedDump) << "Initial dump does not have expected format";
}

TEST_F(TestInterestDump, Redecode)
{
  // check that we encode and decode correctly
  Blob encoding = referenceInterest.wireEncode();
  Interest reDecodedInterest;
  reDecodedInterest.wireDecode(encoding);
  vector<string> redecodedDump = dumpInterest(reDecodedInterest);
  ASSERT_EQ(initialDump, redecodedDump) << "Re-decoded interest does not match original";
}

TEST_F(TestInterestDump, CreateFresh)
{
  Interest freshInterest = createFreshInterest();
  vector<string> freshDump = dumpInterest(freshInterest);
  ASSERT_TRUE(interestDumpsEqual(initialDump, freshDump)) << "Fresh interest does not match original";

  Interest reDecodedFreshInterest;
  reDecodedFreshInterest.wireDecode(freshInterest.wireEncode());
  vector<string> reDecodedFreshDump = dumpInterest(reDecodedFreshInterest);

  ASSERT_TRUE(interestDumpsEqual(freshDump, reDecodedFreshDump)) << "Redecoded fresh interest does not match original";
}


class TestInterestMethods : public ::testing::Test {
public:
  TestInterestMethods()
  : initialDump(initialDumpValues,
      initialDumpValues + sizeof(initialDumpValues) / sizeof(initialDumpValues[0]))
  {
    referenceInterest.wireDecode(codedInterest, sizeof(codedInterest));
  }

  vector<string> initialDump;
  Interest referenceInterest;
};

TEST_F(TestInterestMethods, CopyConstructor)
{
  Interest interest(referenceInterest);
  ASSERT_TRUE(interestDumpsEqual(dumpInterest(interest), dumpInterest(referenceInterest))) <<
    "Interest constructed as deep copy does not match original";
}

TEST_F(TestInterestMethods, EmptyNonce)
{
  // make sure a freshly created interest has no nonce
  Interest freshInterest = createFreshInterest();
  ASSERT_TRUE(freshInterest.getNonce().isNull()) << "Freshly created interest should not have a nonce";
}

TEST_F(TestInterestMethods, SetRemovesNonce)
{
  // Ensure that changing a value on an interest clears the nonce.
  ASSERT_FALSE(referenceInterest.getNonce().isNull());
  Interest interest(referenceInterest);
  // Change a child object.
  interest.getExclude().clear();
  ASSERT_TRUE(interest.getNonce().isNull()) << "Interest should not have a nonce after changing fields";
}

int
main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
