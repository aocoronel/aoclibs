#include <a_types.h>
#include <a_error_types.h>
#include <a_error_fn_ok.h>
#include <a_error_fn_err.h>
#include <a_error_fn_print.h>
#include <a_error_macros.h>
#include <stdio.h>
#include <stdlib.h>
#include <a_error_assert.h>

i32err add_ten(int x) {
        i32 value = x + 10;

        // You can define Error manually
        // Error _err = err(0, "OK");
        // return (i32err){ _err value };

        // Or return an ok
        return (i32err){ ok(), value };
}

Error execute_something(int x) {
        // i32err holds an i32 and Error
        i32err errnval = add_ten(x);
        i32 y = errnval.value;
        // Like ok(), but return an error instead
        if (y > 20) return err(1, NULL);
        return err(0, "OK");
}

int main(int argc, char *argv[]) {
        // This is used only to demonstrate the cleanup macro
        char *mem = malloc(10 * sizeof(char));

        // If execute_something() fails, run free(mem) and return 1
        try_main_cleanup(execute_something(0), free(mem));

        // execute_something() returns an actual error
        Error err = execute_something(120);
        // Asserts err is an error
        ASSERT_ERR(err);
        // Prints error message to stdout
        println_error(err, stdout);

        fflush(stdout);

        // If try_main_cleanup() receives and error, this line is never reached
        free(mem);

        return 0;
}
