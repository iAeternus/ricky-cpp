#ifndef TEST_JSON_SERIALIZER_HPP
#define TEST_JSON_SERIALIZER_HPP

#include "ricky_test.hpp"
#include "json_serializer.hpp"

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

// DEFINE_JSON_ADAPTER(Person);

auto it_works = []() {
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
};

auto should_roundtrip_basic_types = []() {
    json::JsonSerializer js;
    Assertions::assertEquals("123"_s, js.serialize(static_cast<i64>(123)).trim().to_string());
    Assertions::assertEquals("true"_s, js.serialize(true).trim().to_string());
    Assertions::assertEquals("\"hi\""_s, js.serialize("hi"_s).trim().to_string());
};

auto should_roundtrip_container_types = []() {
    json::JsonSerializer js;
    util::Vec<i32> v = {1, 2, 3};
    util::HashMap<util::String, i32> m = {{"a", 1}, {"b", 2}};

    auto v2 = js.deserialize<util::Vec<i32>>(js.serialize(v));
    auto m2 = js.deserialize<util::HashMap<util::String, i32>>(js.serialize(m));

    Assertions::assertEquals(3ULL, v2.len());
    Assertions::assertEquals(2ULL, m2.size());
    Assertions::assertEquals(1, m2.get("a"_s));
};

auto should_roundtrip_person = []() {
    json::JsonSerializer js;
    Person p{"Ricky", 20, true, {100, 90, 80}, {{"city", "Beijing"}, {"street", "No.1"}}};
    auto s = js.serialize(p);
    auto p2 = js.deserialize<Person>(s);
    Assertions::assertEquals(p, p2);
};

auto should_fail_missing_field = []() {
    util::String s = R"({"name":"Ricky","age":20})"_s;
    json::JsonSerializer js;
    Assertions::assertThrows("Missing field in Person json", [&]() {
        auto _ = js.deserialize<Person>(s);
    });
};

auto test_json_serializer() {
    UnitTestGroup group{"test_json_serializer"};

    group.addTest("it_works", it_works);
    group.addTest("should_roundtrip_basic_types", should_roundtrip_basic_types);
    group.addTest("should_roundtrip_container_types", should_roundtrip_container_types);
    group.addTest("should_roundtrip_person", should_roundtrip_person);
    group.addTest("should_fail_missing_field", should_fail_missing_field);

    group.startAll();
}

} // namespace my::test::test_json_serializer

namespace my::json {

using test::test_json_serializer::Person;

auto to_json(Json& j, const Person& p) {
    j = Json::object(
        "name", p.name,
        "age", p.age,
        "is_student", p.is_student,
        "scores", p.scores,
        "address", p.address);
}

auto from_json(const Json& j, Person& p) {
    auto* name = j.find("name");
    auto* age = j.find("age");
    auto* is_student = j.find("is_student");
    auto* scores = j.find("scores");
    auto* address = j.find("address");
    if (!name || !age || !is_student || !scores || !address) {
        throw runtime_exception("Missing field in Person json");
    }
    p.name = name->into<util::String>();
    p.age = age->into<i32>();
    p.is_student = is_student->into<bool>();
    p.scores = scores->into<util::Vec<i32>>();
    p.address = address->into<util::HashMap<util::String, util::String>>();
}

} // namespace my::json

#endif // TEST_JSON_SERIALIZER_HPP
