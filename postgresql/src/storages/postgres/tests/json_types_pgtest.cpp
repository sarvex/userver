#include <storages/postgres/io/json_types.hpp>
#include <storages/postgres/tests/util_pgtest.hpp>

namespace pg = storages::postgres;
namespace io = pg::io;
namespace tt = io::traits;

namespace {

std::string kJsonText = R"~({
  "zulu" : 3.14,
  "foo" : "bar",
  "baz" : [1, 2, 3]
})~";

POSTGRE_TEST_P(JsonSelect) {
  ASSERT_TRUE(conn.get()) << "Expected non-empty connection pointer";

  pg::ResultSet res{nullptr};
  formats::json::Value json;
  auto expected = formats::json::FromString(kJsonText);

  EXPECT_NO_THROW(res = conn->Execute("select '" + kJsonText + "'"));
  EXPECT_NO_THROW(res[0][0].To(json));
  EXPECT_EQ(expected, json);

  EXPECT_NO_THROW(res = conn->Execute("select '" + kJsonText + "'::json"));
  EXPECT_NO_THROW(res[0][0].To(json));
  EXPECT_EQ(expected, json);

  EXPECT_NO_THROW(res = conn->Execute("select '" + kJsonText + "'::jsonb"));
  EXPECT_NO_THROW(res[0][0].To(json));
  EXPECT_EQ(expected, json);
}

POSTGRE_TEST_P(JsonRoundtrip) {
  ASSERT_TRUE(conn.get()) << "Expected non-empty connection pointer";

  pg::ResultSet res{nullptr};
  formats::json::Value json;
  auto expected = formats::json::FromString(kJsonText);

  EXPECT_NO_THROW(res = conn->Execute("select $1", expected));
  EXPECT_NO_THROW(res[0][0].To(json));
  EXPECT_EQ(expected, json);

  EXPECT_NO_THROW(res = conn->Execute("select $1::json", expected));
  EXPECT_NO_THROW(res[0][0].To(json));
  EXPECT_EQ(expected, json);

  EXPECT_NO_THROW(res = conn->Execute("select $1::jsonb", expected));
  EXPECT_NO_THROW(res[0][0].To(json));
  EXPECT_EQ(expected, json);
}

POSTGRE_TEST_P(JsonRoundtripPlain) {
  ASSERT_TRUE(conn.get()) << "Expected non-empty connection pointer";

  pg::ResultSet res{nullptr};
  formats::json::Value json;
  pg::PlainJson expected{formats::json::FromString(kJsonText)};

  EXPECT_NO_THROW(res = conn->Execute("select $1", expected));
  EXPECT_NO_THROW(res[0][0].To(json));
  EXPECT_EQ(expected, json);

  EXPECT_NO_THROW(res = conn->Execute("select $1::json", expected));
  EXPECT_NO_THROW(res[0][0].To(json));
  EXPECT_EQ(expected, json);

  EXPECT_NO_THROW(res = conn->Execute("select $1::jsonb", expected));
  EXPECT_NO_THROW(res[0][0].To(json));
  EXPECT_EQ(expected, json);
}

}  // namespace
