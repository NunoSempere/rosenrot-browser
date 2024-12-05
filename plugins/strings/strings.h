#pragma once

void str_init(char* str, int n);
int str_replace_start(const char* string, const char* target, const char* replacement, char* output);
int str_destructively_omit_from(const char* input, const char* from);
