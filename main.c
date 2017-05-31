#include "parson.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void serialization_example(void) {
    char *serialized_string = NULL;

    JSON_Value *root_value = json_value_init_array();
    JSON_Array *root_object = json_value_get_array(root_value);


    JSON_Value *schema = json_value_init_object();
    JSON_Object *schema_obj = json_value_get_object(schema);
    json_object_set_number(schema_obj, "tamanio", 10);
    json_object_set_number(schema_obj, "disponible", 3);
    //------------Procesos
    JSON_Value *procesos_value = json_value_init_array();
    JSON_Array *procesos_object = json_value_get_array(procesos_value);

    JSON_Value *schema_process = json_value_init_object();
    JSON_Object *schema_process_obj = json_value_get_object(schema_process);
    json_object_set_string(schema_process_obj,"nombre","P1");
    json_object_set_number(schema_process_obj,"utiliza",5);
    json_object_set_number(schema_process_obj,"duracion",30);
    json_array_append_value(procesos_object,schema_process);
    //-----
    json_object_set_value(schema_obj,"procesos",procesos_value);

    
    json_array_append_value(root_object,schema);
    
    serialized_string = json_serialize_to_string_pretty(root_value);
    puts(serialized_string);
    JSON_Value *test_value = json_parse_string("[{\"tamanio\":10,\"disponible\":3,\"procesos\":[{\"name\":\"P1\",\"utiliza\":5,\"tiempo\":30}]},{\"tamanio\":5,\"disponible\":2,\"procesos\":[{\"name\":\"P2\",\"utiliza\":3,\"tiempo\":37}]}]");
    JSON_Array *test_arr = json_value_get_array(test_value);
    //JSON_Array *test_arr = json_object_get_array(test_obj);
    char *serialized_string_01 = NULL;
    serialized_string_01 = json_serialize_to_string_pretty(test_value);
    puts(serialized_string_01);
    size_t count = json_array_get_count(test_arr);
    printf("%d",count);
    json_free_serialized_string(serialized_string);
    json_value_free(root_value);
}

int main()
{
    serialization_example();
    return 0;
}
