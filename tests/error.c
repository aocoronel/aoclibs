#include <aoclibs/common.h>
#include <stdlib.h>

ei32 add_ten(int x) {
        i32 value = x + 10;

        // You can define Err manually
        // Err _err = werr(0, "OK");

        // Or return an ok
        return (ei32){ ok(), value };
}

Err execute_something(int x) {
        // ei32 holds an i32 and Err
        ei32 errval = add_ten(x);
        i32 y = errval.value;
        // Like ok(), but return an error instead
        if (y > 20) return werr(1, NULL);
        return werr(0, "OK");
}

int main(int argc, char *argv[]) {
        Err err = execute_something(120);
        // Asserts err is an error
        ASSERT_ERR(err);
        // Prints error message to stdout
        read_errln(err, stdout);
        return 0;
}
