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

#include "check.h"
#include <stdlib.h>
#include "../cvec.h"


/*
 * Print reverse byte buffer including specified length
 */
int printNBytes(char * receiveBuffer, int num) {
    int i;

    // for (i = num-1; i>=0; i--) {
    //  printf("%02x", (unsigned char) receiveBuffer[i]);
    // }
    for (i =0; i<=num-1; i++) {
        printf("%02x", (unsigned char) receiveBuffer[i]);
    }
    printf("\n");
    return i;
}

void test_write(struct vcLog *myLog, char *expected_message) {
    FILE *file = fopen (myLog->logName, "r");
    char line[1024];
    char *expectedString = "Proc1 {\"Proc1\":2}\n";
    for (int i = 0; i < 3; i++) {
        fgets(line, sizeof(line), file);
    }
    ck_assert_str_eq(line, expectedString);
    fgets(line, sizeof(line), file);
    ck_assert_str_eq(line, expected_message);
    fclose(file);
}

START_TEST(init_cvector_test) {
    char *filename = "testLog";
    struct vcLog *testLog = init_cvector("Proc1", filename);
    FILE *file = fopen (testLog->logName, "r");
    char line[1024];
    char *expectedString = "Proc1 {\"Proc1\":1}\n";
    fgets(line, sizeof(line), file);
    ck_assert_str_eq(line, expectedString);
    expectedString = "Initialization Complete\n";
    fgets(line, sizeof(line), file);
    ck_assert_str_eq(line, expectedString);
    fclose(file);
}
END_TEST

START_TEST(write_log_message_test) {
    char *filename = "testLog";
    struct vcLog *testLog = init_cvector("Proc1", filename);
    FILE *file = fopen (testLog->logName, "r");
    write_log_msg(testLog, "This is a simple log message.");
    char line[1024];
    char *expectedString = "This is a simple log message.\n";
    for (int i = 0; i < 3; i++) {
        fgets(line, sizeof(line), file);
    }
    fgets(line, sizeof(line), file);
    ck_assert_str_eq(line, expectedString);
    fclose(file);
}
END_TEST

START_TEST(log_local_event_test) {
    char *filename = "testLog";
    struct vcLog *testLog = init_cvector("Proc1", filename);
    log_local_event(testLog, "This is a local event!");
    test_write(testLog,"This is a local event!\n");
}
END_TEST

START_TEST(prepare_send_byte_test) {
    char *filename = "testLog";
    struct vcLog *testLog = init_cvector("Proc1", filename);
    char test[1];
    test[0] = 'a';
    int encodeLen;
    char *result = prepare_send(testLog, "Packing Byte.", test, 1, &encodeLen);
    char expected[] = {0xa5, 0x50, 0x72, 0x6f, 0x63, 0x31, 0xc4, 0x01, 0x61, 0x81, 0xa5, 0x50, 0x72, 0x6f, 0x63, 0x31, 0x02};
    ck_assert_mem_eq(result, expected, encodeLen);
    test_write(testLog,"Packing Byte.\n");
}
END_TEST

START_TEST(prepare_send_int_test) {
    char *filename = "testLog";
    struct vcLog *testLog = init_cvector("Proc1", filename);
    int test = 1;
    int encodeLen;
    char *result = prepare_send(testLog, "Packing Integer.", (char *) &test, sizeof(int), &encodeLen);
    char expected[] = {0xa5, 0x50, 0x72, 0x6f, 0x63, 0x31, 0xc4, 0x04, 0x01, 0x00, 0x00, 0x00, 0x81, 0xa5, 0x50, 0x72, 0x6f, 0x63, 0x31, 0x02};
    ck_assert_mem_eq(result, expected, encodeLen);
    test_write(testLog,"Packing Integer.\n");
}
END_TEST

START_TEST(prepare_send_int_array_test) {
    char *filename = "testLog";
    struct vcLog *testLog = init_cvector("Proc1", filename);
    int test[] = {0, 1, 128, 32768, 2147483647};
    int encodeLen;
    char *result = prepare_send(testLog, "Packing Integer Array.", (char *) &test, sizeof(int) * 5, &encodeLen);
    char expected[] = {0xa5, 0x50, 0x72, 0x6f, 0x63, 0x31, 0xc4, 0x14, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0xff, 0xff, 0xff, 0x7f, 0x81, 0xa5, 0x50, 0x72, 0x6f, 0x63, 0x31, 0x02};
    ck_assert_mem_eq(result, expected, encodeLen);
    test_write(testLog,"Packing Integer Array.\n");
}
END_TEST

START_TEST(prepare_send_string_test) {
    char *filename = "testLog";
    struct vcLog *testLog = init_cvector("Proc1", filename);
    char *test = "Test";
    int encodeLen;
    char *result = prepare_send(testLog, "Packing String.", test, strlen(test), &encodeLen);
    char expected[] = {0xa5, 0x50, 0x72, 0x6f, 0x63, 0x31, 0xc4, 0x04, 0x54, 0x65, 0x73, 0x74, 0x81, 0xa5, 0x50, 0x72, 0x6f, 0x63, 0x31, 0x02};
    ck_assert_mem_eq(result, expected, encodeLen);
    test_write(testLog,"Packing String.\n");
}
END_TEST

struct testStruct{
    int integerValue;
    char string[20];
};

START_TEST(prepare_send_struct_test) {
    char *filename = "testLog";
    struct vcLog *testLog = init_cvector("Proc1", filename);
    struct testStruct test = {
        .integerValue = 1234,
        .string = "Information"
    };
    int encodeLen;
    char *result = prepare_send(testLog, "Packing Structure.", (char *) &test, sizeof(struct testStruct), &encodeLen);
    char expected[] = {0xa5, 0x50, 0x72, 0x6f, 0x63, 0x31, 0xc4, 0x18, 0xd2, 0x04, 0x00, 0x00, 0x49, 0x6e, 0x66, 0x6f, 0x72, 0x6d, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x81, 0xa5, 0x50, 0x72, 0x6f, 0x63, 0x31, 0x02};
    ck_assert_mem_eq(result, expected, encodeLen);
    test_write(testLog,"Packing Structure.\n");
}
END_TEST

START_TEST(unpack_receive_byte_test) {
    char *filename = "testLog";
    struct vcLog *testLog = init_cvector("Proc1", filename);
    int bufferLen = 256;
    char expected = 'a';
    char input[] = {0xa5, 0x50, 0x72, 0x6f, 0x63, 0x31, 0xc4, 0x01, 0x61, 0x81, 0xa5, 0x50, 0x72, 0x6f, 0x63, 0x31, 0x02};
    int contentLen;
    char *result = unpack_receive(testLog, "Unpacking Byte.", input, bufferLen, &contentLen);
    ck_assert_mem_eq(result, &expected, contentLen);
    test_write(testLog,"Unpacking Byte.\n");
}
END_TEST

START_TEST(unpack_receive_int_test) {
    char *filename = "testLog";
    struct vcLog *testLog = init_cvector("Proc1", filename);
    int bufferLen = 256;
    int expected = 1;
    char input[] = {0xa5, 0x50, 0x72, 0x6f, 0x63, 0x31, 0xc4, 0x04, 0x01, 0x00, 0x00, 0x00, 0x81, 0xa5, 0x50, 0x72, 0x6f, 0x63, 0x31, 0x02};
    int contentLen;
    char *result = unpack_receive(testLog, "Unpacking Integer.", input, bufferLen, &contentLen);
    ck_assert_int_eq(*result, expected);
    test_write(testLog,"Unpacking Integer.\n");
}
END_TEST

START_TEST(unpack_receive_int_array_test) {
    char *filename = "testLog";
    struct vcLog *testLog = init_cvector("Proc1", filename);
    int bufferLen = 256;
    int expected[] = {0, 1, 128, 32768, 2147483647};
    char input[] = {0xa5, 0x50, 0x72, 0x6f, 0x63, 0x31, 0xc4, 0x14, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0xff, 0xff, 0xff, 0x7f, 0x81, 0xa5, 0x50, 0x72, 0x6f, 0x63, 0x31, 0x02};
    int contentLen;
    char *result = unpack_receive(testLog, "Unpacking Integer Array.", input, bufferLen, &contentLen);
    ck_assert_mem_eq(result, expected, contentLen);
    test_write(testLog,"Unpacking Integer Array.\n");
}
END_TEST

START_TEST(unpack_receive_string_test) {
    char *filename = "testLog";
    struct vcLog *testLog = init_cvector("Proc1", filename);
    int bufferLen = 256;
    char *expected = "Test";
    char input[] = {0xa5, 0x50, 0x72, 0x6f, 0x63, 0x31, 0xc4, 0x04, 0x54, 0x65, 0x73, 0x74, 0x81, 0xa5, 0x50, 0x72, 0x6f, 0x63, 0x31, 0x02};
    int contentLen;
    char *result = unpack_receive(testLog, "Unpacking String.", input, bufferLen, &contentLen);
    ck_assert_str_eq(result, expected);
    test_write(testLog,"Unpacking String.\n");
}
END_TEST

START_TEST(unpack_receive_struct_test) {
    char *filename = "testLog";
    struct vcLog *testLog = init_cvector("Proc1", filename);
    int bufferLen = 256;
    char *expected_str = "Information";
    int expected_int = 1234;
    char input[] = {0xa5, 0x50, 0x72, 0x6f, 0x63, 0x31, 0xc4, 0x18, 0xd2, 0x04, 0x00, 0x00, 0x49, 0x6e, 0x66, 0x6f, 0x72, 0x6d, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x81, 0xa5, 0x50, 0x72, 0x6f, 0x63, 0x31, 0x02};
    int contentLen;
    char *result = unpack_receive(testLog, "Unpacking Struct.", input, bufferLen, &contentLen);
    struct testStruct *resultStruct = (struct testStruct *) result;
    ck_assert_int_eq(resultStruct->integerValue, expected_int);
    ck_assert_str_eq(resultStruct->string, expected_str);

    test_write(testLog,"Unpacking Struct.\n");
}
END_TEST

START_TEST(disable_logging_test) {
    char *filename = "testLog";
    struct vcLog *testLog = init_cvector("Proc1", filename);
    disable_logging(testLog);
    log_local_event(testLog, "This should not be logged!");
    FILE *file = fopen (testLog->logName, "r");
    char line[1024];
    for (int i = 0; i < 2; i++) {
        fgets(line, sizeof(line), file);
    }
    ck_assert_ptr_null(fgets(line, sizeof(line), file));
    fclose(file);
}
END_TEST

START_TEST(enable_logging_test) {
    char *filename = "testLog";
    struct vcLog *testLog = init_cvector("Proc1", filename);
    disable_logging(testLog);
    log_local_event(testLog, "This should not be logged!");
    enable_logging(testLog);
    write_log_msg(testLog, "This should be logged.");
    FILE *file = fopen (testLog->logName, "r");
    char line[1024];
    char *expectedString = "Proc1 {\"Proc1\":2}\n";
    for (int i = 0; i < 3; i++) {
        fgets(line, sizeof(line), file);
    }
    ck_assert_str_eq(line, expectedString);
    expectedString = "This should be logged.\n";
    fgets(line, sizeof(line), file);
    ck_assert_str_eq(line, expectedString);
    fclose(file);
}
END_TEST

Suite *cvec_suite(void) {
    Suite *s;
    TCase *basic_tests;
    s = suite_create("cvec");
    /* Basic test case */
    basic_tests = tcase_create("Basic");
    /*Current test cases */
    tcase_add_test(basic_tests, init_cvector_test);
    tcase_add_test(basic_tests, write_log_message_test);
    tcase_add_test(basic_tests, log_local_event_test);
    tcase_add_test(basic_tests, prepare_send_byte_test);
    tcase_add_test(basic_tests, prepare_send_int_test);
    tcase_add_test(basic_tests, prepare_send_int_array_test);
    tcase_add_test(basic_tests, prepare_send_string_test);
    tcase_add_test(basic_tests, prepare_send_struct_test);
    tcase_add_test(basic_tests, unpack_receive_byte_test);
    tcase_add_test(basic_tests, unpack_receive_int_test);
    tcase_add_test(basic_tests, unpack_receive_int_array_test);
    tcase_add_test(basic_tests, unpack_receive_string_test);
    tcase_add_test(basic_tests, unpack_receive_string_test);
    tcase_add_test(basic_tests, unpack_receive_struct_test);
    tcase_add_test(basic_tests, disable_logging_test);
    tcase_add_test(basic_tests, enable_logging_test);

    suite_add_tcase(s, basic_tests);
    return s;
}

int main(void) {
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = cvec_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    remove ("testLog-shiviz.txt"); // Clean the file
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}