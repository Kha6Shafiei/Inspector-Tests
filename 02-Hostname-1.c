#if 0
    source "${TEST_DIR}/lib/crunner" -linspector
#endif

#include <limits.h>
#include <sys/stat.h>
#include <unistd.h>

#include "procfs.h"
#include "build_path.h"

test_start(
        "Verifies that the pfs_hostname function retrieves the system "
        "hostname correctly.");

subtest("Comparing output against gethostname(3)",
{
    char expected_hname[HOST_NAME_MAX + 1] = { 88 };
    /* We'll use gethostname(3) as our reference function: */
    gethostname(expected_hname, HOST_NAME_MAX);

    char hostname[HOST_NAME_MAX + 1] = { 88 };
    int return_value = pfs_hostname("/proc", hostname, HOST_NAME_MAX + 1);

    test_assert_str(hostname, "==", expected_hname, HOST_NAME_MAX + 1);
    test_printf("'%s'", hostname);

    test_assert(return_value == 0);
    test_printf("%d", return_value);
});


subtest("Comparing against fake procfs test set",
{
    char *fakeproc = build_path(getenv("TEST_DIR"), "/inputs/fakeproc");
    char hostname[HOST_NAME_MAX + 1] = { 88 };
    int return_value = pfs_hostname(fakeproc, hostname, HOST_NAME_MAX + 1);
    free(fakeproc);

    test_assert_str(hostname, "==", "deus-ex-machina", HOST_NAME_MAX + 1);
    test_printf("'%s'", hostname);

    test_assert(return_value == 0);
    test_printf("%d", return_value);
});


subtest("Invalid proc location. Return value should be nonzero.",
{
    char invalid[4096] = "/this/does/not/exist";
    char hostname[HOST_NAME_MAX + 1] = { 88 };
    int return_value = pfs_hostname(invalid, hostname, HOST_NAME_MAX + 1);

    test_assert(return_value != 0);
    test_printf("%d", return_value);
});


subtest("Unreadable hostname file. Return value should be nonzero.",
{
    char *dummyproc = build_path(getenv("TEST_DIR"), "/inputs/dummyproc1");
    char *hn_file = build_path(getenv("TEST_DIR"),
            "/inputs/dummyproc1/sys/kernel/hostname");
    int chmod_ret = chmod(hn_file, 0000);
    test_assert(chmod_ret == 0, "If this fails the test dataset is broken");
    free(hn_file);

    char hostname[HOST_NAME_MAX + 1] = { 88 };
    int return_value = pfs_hostname(dummyproc, hostname, HOST_NAME_MAX + 1);

    test_assert(return_value != 0);
    test_printf("%d", return_value);
});

test_end
