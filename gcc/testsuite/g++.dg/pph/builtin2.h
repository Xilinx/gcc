const char *date = VALUE(__DATE__);
const char *time = VALUE(__TIME__);
const char *file = VALUE(__FILE__);
const char *line = VALUE(__LINE__);
const char *vers = VALUE(__cplusplus);
const char *optm = VALUE(__OPTIMIZE__);
const char *func() { return __func__; }
