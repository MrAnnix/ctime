# ctime

Usage: ctime [OPTION]... [TIMESTAMP]
Converts Unix timestamp to human readable time.

With no TIMESTAMP prints the actual time.

Options:
        -h, --help           Displays this help and exit
        -v, --version        Outputs version information and exit
            --ms             When TIMESTAMP is in miliseconds
            --tz TIMEZONE    Outputs the date in the corresponding TIMEZONE according to tz database https://en.wikipedia.org/wiki/List_of_tz_database_time_zones
                             If TIMEZONE is not specified, the default system timezone will be used.
