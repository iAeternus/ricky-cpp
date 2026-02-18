#include "test_json_serializer.hpp"
#include "json_serializer.hpp"
#include "printer.hpp"
#include "ricky_test.hpp"

namespace my::test::test_json_serializer {

struct Person : Object<Person> {
    using Self = Person;

    util::String name;
    i32 age;
    bool is_student;
    util::Vec<i32> scores;
    util::HashMap<util::String, util::String> address;

    Person() = default;

    Person(util::String name, i32 age, bool is_student, const util::Vec<i32>& scores, const util::HashMap<util::String, util::String>& address) :
            name(std::move(name)), age(age), is_student(is_student), scores(scores), address(address) {}

    [[nodiscard]] cmp_t __cmp__(const Self& other) const {
        return name.__cmp__(other.name);
    }
};

inline void to_json(json::Json& j, const Person& p) {
    j = json::Json::object();
    j.insert("name", p.name);
    j.insert("age", p.age);
    j.insert("is_student", p.is_student);
    json::Json scores_json;
    json::Json address_json;
    to_json(scores_json, p.scores);
    to_json(address_json, p.address);
    j.insert("scores", std::move(scores_json));
    j.insert("address", std::move(address_json));
}

inline void from_json(const json::Json& j, Person& p) {
    if (!j.contains("name") || !j.contains("age") || !j.contains("is_student") || !j.contains("scores") || !j.contains("address")) {
        throw runtime_exception("Missing field in Person json");
    }
    p.name = j["name"].into<util::String>();
    p.age = j["age"].into<i32>();
    p.is_student = j["is_student"].into<bool>();
    p.scores = j["scores"].into<util::Vec<i32>>();
    p.address = j["address"].into<util::HashMap<util::String, util::String>>();
}

void it_works() {
    // Given
    Person p{"Ricky", 20, true, {100, 90, 80}, {{"city", "Beijing"}, {"street", "No.1"}}};
    util::String s = R"({"name":"Ricky","age":20,"is_student":true,"scores":[100,90,80],"address":{"city":"Beijing","street":"No.1"}})";
    json::JsonSerializer js;

    // When
    auto res = js.serialize(p);
    io::println(res);
    auto p2 = js.deserialize<Person>(s);

    // Then
    Assertions::assertEquals(p, p2);
}

void should_roundtrip_basic_types() {
    json::JsonSerializer js;
    Assertions::assertEquals("123"_s, js.serialize(static_cast<i64>(123)).trim().to_string());
    Assertions::assertEquals("true"_s, js.serialize(true).trim().to_string());
    Assertions::assertEquals("\"hi\""_s, js.serialize("hi"_s).trim().to_string());
}

void should_roundtrip_container_types() {
    json::JsonSerializer js;
    util::Vec<i32> v = {1, 2, 3};
    util::HashMap<util::String, i32> m = {{"a", 1}, {"b", 2}};

    auto v2 = js.deserialize<util::Vec<i32>>(js.serialize(v));
    auto m2 = js.deserialize<util::HashMap<util::String, i32>>(js.serialize(m));

    Assertions::assertEquals(3ULL, v2.len());
    Assertions::assertEquals(2ULL, m2.size());
    Assertions::assertEquals(1, m2.get("a"_s));
}

void should_roundtrip_person() {
    json::JsonSerializer js;
    Person p{"Ricky", 20, true, {100, 90, 80}, {{"city", "Beijing"}, {"street", "No.1"}}};
    auto s = js.serialize(p);
    auto p2 = js.deserialize<Person>(s);
    Assertions::assertEquals(p, p2);
}

void should_fail_missing_field() {
    util::String s = R"({"name":"Ricky","age":20})"_s;
    json::JsonSerializer js;
    Assertions::assertThrows("Missing field in Person json", [&]() {
        auto _ = js.deserialize<Person>(s);
    });
}

GROUP_NAME("test_json_serializer")
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(it_works),
    UNIT_TEST_ITEM(should_roundtrip_basic_types),
    UNIT_TEST_ITEM(should_roundtrip_container_types),
    UNIT_TEST_ITEM(should_roundtrip_person),
    UNIT_TEST_ITEM(should_fail_missing_field))

} // namespace my::test::test_json_serializer
