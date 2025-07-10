#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    TOKEN_STRING,
    TOKEN_NUMBER,
    TOKEN_TRUE,
    TOKEN_FALSE,
    TOKEN_NULL,
    TOKEN_OBJECT_START,
    TOKEN_OBJECT_END,
    TOKEN_ARRAY_START,
    TOKEN_ARRAY_END,
    TOKEN_COLON,
    TOKEN_COMMA,
    TOKEN_ERROR,
    TOKEN_EOF
} TokenType;

typedef struct {
    TokenType type;
    char value[100];
} Token;

typedef struct {
    char key[100];
    char value[100];
} KeyValuePair;

typedef struct {
    KeyValuePair* pairs;
    size_t count;
} JSONObject;

JSONObject* parse_json(const char** json_string);

Token get_next_token(const char** json_string) {
    Token token;
    token.value[0] = '\0';

    while (**json_string == ' ' || **json_string == '\t' || **json_string == '\n' || **json_string == '\r') {
        (*json_string)++;
    }

    if (**json_string == '\0') {
        token.type = TOKEN_EOF;
        return token;
    }

    if (**json_string == '\"') {
        (*json_string)++;
        token.type = TOKEN_STRING;

        size_t i = 0;
        while (**json_string != '\"' && **json_string != '\0') {
            token.value[i++] = **json_string;
            (*json_string)++;
        }

        if (**json_string == '\"') {
            (*json_string)++;
        }

        token.value[i] = '\0';
        return token;
    }

    if ((**json_string >= '0' && **json_string <= '9') || **json_string == '-') {
        token.type = TOKEN_NUMBER;

        size_t i = 0;
        while ((**json_string >= '0' && **json_string <= '9') || **json_string == '.') {
            token.value[i++] = **json_string;
            (*json_string)++;
        }

        token.value[i] = '\0';
        return token;
    }

    if (strncmp(*json_string, "true", 4) == 0) {
        token.type = TOKEN_TRUE;
        strncpy(token.value, "true", 4);
        (*json_string) += 4;
    } else if (strncmp(*json_string, "false", 5) == 0) {
        token.type = TOKEN_FALSE;
        strncpy(token.value, "false", 5);
        (*json_string) += 5;
    } else if (strncmp(*json_string, "null", 4) == 0) {
        token.type = TOKEN_NULL;
        strncpy(token.value, "null", 4);
        (*json_string) += 4;
    } else if (**json_string == '{') {
        token.type = TOKEN_OBJECT_START;
        token.value[0] = '{';
        token.value[1] = '\0';
        (*json_string)++;
    } else if (**json_string == '}') {
        token.type = TOKEN_OBJECT_END;
        token.value[0] = '}';
        token.value[1] = '\0';
        (*json_string)++;
    } else if (**json_string == '[') {
        token.type = TOKEN_ARRAY_START;
        token.value[0] = '[';
        token.value[1] = '\0';
        (*json_string)++;
    } else if (**json_string == ']') {
        token.type = TOKEN_ARRAY_END;
        token.value[0] = ']';
        token.value[1] = '\0';
        (*json_string)++;
    } else if (**json_string == ':') {
        token.type = TOKEN_COLON;
        token.value[0] = ':';
        token.value[1] = '\0';
        (*json_string)++;
    } else if (**json_string == ',') {
        token.type = TOKEN_COMMA;
        token.value[0] = ',';
        token.value[1] = '\0';
        (*json_string)++;
    } else {
        token.type = TOKEN_ERROR;
        token.value[0] = **json_string;
        token.value[1] = '\0';
        (*json_string)++;
    }

    return token;
}

JSONObject* parse_json_object(const char** json_string) {
    JSONObject* obj = (JSONObject*)malloc(sizeof(JSONObject));
    obj->count = 0;
    obj->pairs = NULL;

    Token token;
    /* Fix Chat GPT: Comment code below, it's already done on top level */
    //if (token.type != TOKEN_OBJECT_START) {
    //    printf("Error: Expected '{' at the beginning of the object.\n");
    //    free(obj);
    //    return NULL;
    //}

    while (1) {
        token = get_next_token(json_string);

        if (token.type == TOKEN_STRING) {
            char key[100];
            strcpy(key, token.value);

            token = get_next_token(json_string);
            if (token.type != TOKEN_COLON) {
                printf("Error: Expected ':' after the key.\n");
                free(obj);
                return NULL;
            }

            JSONObject* value = parse_json(json_string);
            if (value != NULL) {
                KeyValuePair pair;
                strcpy(pair.key, key);
                strcpy(pair.value, value->pairs[0].value);

                obj->pairs = (KeyValuePair*)realloc(obj->pairs, (obj->count + 1) * sizeof(KeyValuePair));
                obj->pairs[obj->count++] = pair;

                free(value);
            } else {
                free(obj);
                return NULL;
            }

            token = get_next_token(json_string);

            if (token.type == TOKEN_COMMA) {
                continue;
            } else if (token.type == TOKEN_OBJECT_END) {
                break;
            } else {
                printf("Error: Expected ',' or '}' after a key-value pair.\n");
                free(obj);
                return NULL;
            }
        } else if (token.type == TOKEN_OBJECT_END) {
            break;
        } else {
            printf("Error: Expected a key in the object.\n");
            free(obj);
            return NULL;
        }
    }

    return obj;
}

JSONObject* parse_json_array(const char** json_string) {
    JSONObject* arr = (JSONObject*)malloc(sizeof(JSONObject));
    arr->count = 0;
    arr->pairs = NULL;

    Token token;
    /* Fix Chat GPT: Comment code below, it's already done on top level */
    //if (token.type != TOKEN_ARRAY_START) {
    //    printf("Error: Expected '[' at the beginning of the array.\n");
    //    free(arr);
    //    return NULL;
    //}

    while (1) {
        JSONObject* value = parse_json(json_string);
        if (value != NULL) {
            KeyValuePair pair;
            strcpy(pair.key, "");
            strcpy(pair.value, value->pairs[0].value);

            arr->pairs = (KeyValuePair*)realloc(arr->pairs, (arr->count + 1) * sizeof(KeyValuePair));
            arr->pairs[arr->count++] = pair;

            free(value);
        } else {
            free(arr);
            return NULL;
        }

        token = get_next_token(json_string);

        if (token.type == TOKEN_COMMA) {
            continue;
        } else if (token.type == TOKEN_ARRAY_END) {
            break;
        } else {
            printf("Error: Expected ',' or ']' after an array element.\n");
            free(arr);
            return NULL;
        }
    }

    return arr;
}

JSONObject* parse_json(const char** json_string) {
    Token token = get_next_token(json_string);

    switch (token.type) {
        case TOKEN_STRING: {
            JSONObject* obj = (JSONObject*)malloc(sizeof(JSONObject));
            obj->count = 1;
            obj->pairs = (KeyValuePair*)malloc(sizeof(KeyValuePair));

            KeyValuePair pair;
            strcpy(pair.key, "");
            strcpy(pair.value, token.value);

            obj->pairs[0] = pair;
            return obj;
        }
        case TOKEN_NUMBER: {
            JSONObject* obj = (JSONObject*)malloc(sizeof(JSONObject));
            obj->count = 1;
            obj->pairs = (KeyValuePair*)malloc(sizeof(KeyValuePair));

            KeyValuePair pair;
            strcpy(pair.key, "");
            strcpy(pair.value, token.value);

            obj->pairs[0] = pair;
            return obj;
        }
        case TOKEN_TRUE: {
            JSONObject* obj = (JSONObject*)malloc(sizeof(JSONObject));
            obj->count = 1;
            obj->pairs = (KeyValuePair*)malloc(sizeof(KeyValuePair));

            KeyValuePair pair;
            strcpy(pair.key, "");
            strcpy(pair.value, "true");

            obj->pairs[0] = pair;
            return obj;
        }
        case TOKEN_FALSE: {
            JSONObject* obj = (JSONObject*)malloc(sizeof(JSONObject));
            obj->count = 1;
            obj->pairs = (KeyValuePair*)malloc(sizeof(KeyValuePair));

            KeyValuePair pair;
            strcpy(pair.key, "");
            strcpy(pair.value, "false");

            obj->pairs[0] = pair;
            return obj;
        }
        case TOKEN_NULL: {
            JSONObject* obj = (JSONObject*)malloc(sizeof(JSONObject));
            obj->count = 1;
            obj->pairs = (KeyValuePair*)malloc(sizeof(KeyValuePair));

            KeyValuePair pair;
            strcpy(pair.key, "");
            strcpy(pair.value, "null");

            obj->pairs[0] = pair;
            return obj;
        }
        case TOKEN_OBJECT_START: {
            return parse_json_object(json_string);
        }
        case TOKEN_ARRAY_START: {
            return parse_json_array(json_string);
        }
        default: {
            printf("Error: Unexpected token type: %d\n", token.type);
            return NULL;
        }
    }
}

int main(int argc, char** argv) {
    FILE* fp;
    char * json_string;
    int ret;

    /* Non chat GPT code: argument handling and reading content from file */
    if (argc < 2) {
        printf("Usage: %s json-file\n", argv[0]);
        return -1;
    }

    fp = fopen(argv[1], "r");

 
    if (fp == NULL ) {
        printf("Couldn't open json file\n");
        return -1;
    }
 
    json_string = malloc(4096);
    ret = fread(json_string, 1, 4096, fp);

    if (ret == -1) {
        printf("Reading json file failed!\n");
        return -1;
    }

    fclose(fp);


    JSONObject* obj = parse_json((const char **) &json_string);

    if (obj != NULL) {
        printf("Parsed JSON:\n");
        for (size_t i = 0; i < obj->count; ++i) {
            printf("%s: %s\n", obj->pairs[i].key, obj->pairs[i].value);
        }
        free(obj->pairs);
        free(obj);
    }

    return 0;
}
