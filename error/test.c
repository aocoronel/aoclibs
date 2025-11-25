#include <a_int.h>
#include <a_error.h>
#include <stdio.h>
#include <stdlib.h>
#include <a_assert.h>

i32err add_ten(int x) {
        Error _err = err(0, "OK");
        i32 value = x + 10;
        return (i32err){_err, value};
}

Error execute_something(int x) {
        i32err errnval = add_ten(x);
        i32 y = errnval.value;
        if (y > 20) return err(1, NULL);
        return err(0, "OK");
}

int main(int argc, char *argv[]) {
        char *mem = malloc(10 * sizeof(char)); // Allocates...

        // If execute_something() fails, run free(mem) and return 1
        try_main_cleanup(execute_something(0), free(mem));

        // execute_something() returns an actual error
        Error err = execute_something(120);
        // Asserts err is an error
        assert_err(err);

        // Prints error message to stdout
        print_error(err, stdout);

        fflush(stdout);

        // If try_main_cleanup() receives and error, this line is never reached
        free(mem);

        return 0;
}
