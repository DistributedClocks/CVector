 /*
  * Check: a unit test framework for C
  * Copyright (C) 2001, 2002 Arien Malec
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the GNU Lesser General Public
  * License as published by the Free Software Foundation; either
  * version 2.1 of the License, or (at your option) any later version.
  *
  * This library is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  * Lesser General Public License for more details.
  *
  * You should have received a copy of the GNU Lesser General Public
  * License along with this library; if not, write to the
  * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
  * MA 02110-1301, USA.
  */

#include <check.h>
#include "../vclock/vclock.h"

START_TEST(clock_init_test) {
    struct vectorClock *testClock = clock_init("Proc1");
    ck_assert_int_eq(testClock->time, 0);
    ck_assert_str_eq(testClock->id, "Proc1");
}
END_TEST

START_TEST(tick_test) {
    struct vectorClock *testClock = clock_init("Proc1");
    struct vectorClock *iterClock = testClock;
    tick(&testClock, "Proc1");
    tick(&testClock, "Proc2");
    tick(&testClock, "");
    ck_assert_int_eq(iterClock->time, 1);
    ck_assert_str_eq(iterClock->id, "Proc1");
    iterClock=(struct vectorClock *)(iterClock->hh.next);
    ck_assert_int_eq(iterClock->time, 1);
    ck_assert_str_eq(iterClock->id, "Proc2");
    iterClock=(struct vectorClock *)(iterClock->hh.next);
    ck_assert_int_eq(iterClock->time, 1);
    ck_assert_str_eq(iterClock->id, "");
}
END_TEST

START_TEST(set_test) {
    struct vectorClock *testClock = clock_init("Proc1");
    struct vectorClock *iterClock = testClock;
    set(&testClock, "Proc1", 1);
    ck_assert_int_eq(iterClock->time, 1);
    ck_assert_str_eq(iterClock->id, "Proc1");    
    set(&testClock, "Proc1", 10);
    ck_assert_int_eq(iterClock->time, 10);
    ck_assert_str_eq(iterClock->id, "Proc1");
    set(&testClock, "Proc1", 0);
    ck_assert_int_eq(iterClock->time, 1);
    ck_assert_str_eq(iterClock->id, "Proc1");
    set(&testClock, "Proc2", 2);
    iterClock=(struct vectorClock *)(iterClock->hh.next);
    ck_assert_int_eq(iterClock->time, 2);
    ck_assert_str_eq(iterClock->id, "Proc2");

}
END_TEST

START_TEST(find_ticks_test) {
    struct vectorClock *testClock = clock_init("Proc1");
    tick(&testClock, "Proc1");
    set(&testClock, "Proc2", 50);
    ck_assert_int_eq(find_ticks(testClock, "Proc1"), 1);
    ck_assert_int_eq(find_ticks(testClock, "Proc2"), 50);
    ck_assert_int_eq(find_ticks(testClock, "Proc3"), -1);
}
END_TEST

START_TEST(copy_test) {
    struct vectorClock *testClock = NULL;
    set(&testClock, "Proc1", 50);
    struct vectorClock *copyClock = copy(testClock);
    ck_assert_int_eq(find_ticks(testClock, "Proc1"), 50);
    ck_assert_int_eq(find_ticks(copyClock, "Proc1"), 50);
}
END_TEST

START_TEST(last_update_test) {
    struct vectorClock *testClock = NULL;
    set(&testClock, "Proc1", 2);
    set(&testClock, "Proc3", 3);
    set(&testClock, "Proc2", 7);
    ck_assert_int_eq(last_update(testClock), 7);
}
END_TEST

START_TEST(return_vc_string_test) {
    struct vectorClock *testClock = NULL;
    set(&testClock, "Proc1", 1);
    set(&testClock, "Proc5", 6);
    set(&testClock, "Proc2", 2);
    set(&testClock, "", 0);
    char *resultString = return_vc_string(testClock);
    char *expectedString = "{\"Proc1\":1, \"Proc5\":6, \"Proc2\":2, \"\":1}";
    ck_assert_str_eq(resultString, expectedString);
    free(resultString);
}
END_TEST

START_TEST(merge_test) {
    struct vectorClock *testClock = NULL;
    struct vectorClock *testClock2 = NULL;

    set(&testClock, "Proc1", 1);
    set(&testClock, "Proc2", 2);
    set(&testClock2, "Proc2", 3);
    set(&testClock2, "Proc3", 1);
    struct vectorClock *mergeClock = copy(testClock);

    merge(mergeClock, testClock2);
    char *resultString = return_vc_string(mergeClock);
    char *expectedString = "{\"Proc1\":1, \"Proc2\":3, \"Proc3\":1}";
    ck_assert_str_eq(resultString, expectedString);
    free(resultString);
}
END_TEST

START_TEST(print_vc_test) {
    char output[256];
    struct vectorClock *testClock = NULL;

    set(&testClock, "Proc1", 2);
    set(&testClock, "Proc3", 2);
    set(&testClock, "Proc2", 7);
    freopen("/dev/null", "a", stdout);
    setbuf(stdout, output);
    print_vc(testClock);
    char *expectedString = "{\"Proc1\":2, \"Proc3\":2, \"Proc2\":7}\n";
    ck_assert_str_eq(output, expectedString);
    freopen("/dev/tty", "a", stdout);
}
END_TEST

START_TEST(sort_by_id_test) {
    struct vectorClock *testClock = NULL;
    set(&testClock, "Proc1", 1);
    set(&testClock, "Proc5", 6);
    set(&testClock, "Proc2", 2);
    set(&testClock, "", 0);
    sort_by_id(&testClock);
    char *resultString = return_vc_string(testClock);
    char *expectedString = "{\"\":1, \"Proc1\":1, \"Proc2\":2, \"Proc5\":6}";
    ck_assert_str_eq(resultString, expectedString);
    free(resultString);
}
END_TEST

START_TEST(sort_by_time_test) {
    struct vectorClock *testClock = NULL;
    set(&testClock, "Proc1", 1);
    set(&testClock, "Proc5", 6);
    set(&testClock, "Proc2", 2);
    set(&testClock, "", 0);
    sort_by_time(&testClock);
    char *resultString = return_vc_string(testClock);
    char *expectedString = "{\"Proc1\":1, \"\":1, \"Proc2\":2, \"Proc5\":6}";
    ck_assert_str_eq(resultString, expectedString);
    free(resultString);
}
END_TEST

START_TEST(copy_sort_test) {
    struct vectorClock *testClock = NULL;
    set(&testClock, "Proc1", 1);
    set(&testClock, "Proc5", 6);
    set(&testClock, "Proc2", 2);
    set(&testClock, "", 0);
    struct vectorClock *copyClock = copy_sort(testClock);

    char *resultString = return_vc_string(copyClock);
    char *expectedString = "{\"\":1, \"Proc1\":1, \"Proc2\":2, \"Proc5\":6}";
    ck_assert_str_eq(resultString, expectedString);
    free(resultString);
}
END_TEST

Suite *vclock_suite(void) {
    Suite *s;
    TCase *basic_tests;
    s = suite_create("vclock");
    /* Basic test case */
    basic_tests = tcase_create("Basic");
    /*Current test cases */
    tcase_add_test(basic_tests, clock_init_test);
    tcase_add_test(basic_tests, tick_test);
    tcase_add_test(basic_tests, set_test);
    tcase_add_test(basic_tests, find_ticks_test);
    tcase_add_test(basic_tests, copy_test);
    tcase_add_test(basic_tests, last_update_test);
    tcase_add_test(basic_tests, return_vc_string_test);
    tcase_add_test(basic_tests, merge_test);
    tcase_add_test(basic_tests, print_vc_test);
    tcase_add_test(basic_tests, sort_by_id_test);
    tcase_add_test(basic_tests, sort_by_time_test);
    tcase_add_test(basic_tests, copy_sort_test);

    suite_add_tcase(s, basic_tests);
    return s;
}

int main(void) {
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = vclock_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}