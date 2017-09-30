/*
 * Copyright (C) 2017 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/

#include <iostream>
#include <string>
#include <vector>

#include <json/json.h>

#include <ignition/common/Console.hh>
#include "ignition/fuel-tools/JSONParser.hh"

using namespace ignition;
using namespace fuel_tools;

#if defined(_WIN32) && !defined(timegm)
  #define timegm _mkgmtime
#endif

/////////////////////////////////////////////////
std::time_t ParseDateTime(const std::string &_datetime)
{
  int y,M,d,h,m;
  float s;
  sscanf(_datetime.c_str(), "%d-%d-%dT%d:%d:%fZ", &y, &M, &d, &h, &m, &s);
  std::tm tm;
  // Year since 1900
  tm.tm_year = y - 1900;
  // 0-11
  tm.tm_mon = M - 1;
  // 1-31
  tm.tm_mday = d;
  // 0-23
  tm.tm_hour = h;
  // 0-59
  tm.tm_min = m;
  // 0-61 (0-60 in C++11)
  tm.tm_sec = (int)s;
  // 0 - standard time, 1, daylight saving, -1 unknown
  tm.tm_isdst = -1;
  return timegm(&tm);
}

/////////////////////////////////////////////////
std::vector<ModelIdentifier> JSONParser::ParseModels(const std::string &_json)
{
  std::vector<ModelIdentifier> ids;
  Json::Reader reader;
  Json::Value models;
  reader.parse(_json, models);

  try
  {
    if (!models.isArray())
    {
      ignerr << "JSON response is not an array\n";
    }
    else
    {
      for (auto modelIt = models.begin(); modelIt != models.end(); ++modelIt)
      {
        Json::Value model = *modelIt;
        if (!model.isObject())
        {
          ignerr << "Model isn't a json object!\n";
          break;
        }
        ModelIdentifier id;

        if (model.isMember("name"))
          id.Name(model["name"].asString());
        if (model.isMember("owner"))
          id.Owner(model["owner"].asString());
        else
          id.Owner("anonymous");
        if (model.isMember("uuid"))
          id.Uuid(model["uuid"].asString());
        if (model.isMember("updatedAt"))
          id.ModifyDate(ParseDateTime(model["updatedAt"].asString()));
        if (model.isMember("createdAt"))
          id.UploadDate(ParseDateTime(model["createdAt"].asString()));

        ids.push_back(id);
      }
    }
  }
#if JSONCPP_VERSION_MAJOR < 1 && JSONCPP_VERSION_MINOR < 10
  catch (...)
  {
    std::string what;
#else
  catch (const Json::LogicError &error)
  {
    std::string what = ": [" + std::string(error.what()) + "]";
#endif
    ignerr << "Bad response from server" << what << "\n";
  }

  return ids;
}

/////////////////////////////////////////////////
std::string JSONParser::BuildModel(ModelIter _modelIt)
{
  ModelIdentifier id = _modelIt->Identification();
  Json::Value value;
  value["name"] = id.Name();
  value["description"] = id.Description();
  value["category"] = id.Category();
  value["uuid"] = id.Uuid();

  Json::StyledWriter writer;
  return writer.write(value);
}