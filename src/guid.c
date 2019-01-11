#include <guid.h>

const char * charset[] = {'a', 'b', 'c', 'd', 'e', 'f', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '\0'};

void create_guid(char guid[], guid_t* out)
{
    int i = 0;
    
    while(guid[i] != '\0'){
        if(i == 8 || i == 13 || i == 18 || i == 23)
            out->value[i] = '-';
        else
            out->value[i] = guid[i];
        i++;
    }
    out->value[36] = '\0';
}

void create_new_guid(guid_t* out)
{
    int i = 0;
    srand(time(NULL));

    while(i < 36) {
        if(i == 8 || i == 13 || i == 18 || i == 23)
            out->value[i] = '-';
        else
            out->value[i] = charset[(random() % 16)];
        i++;
    }
    out->value[36] = '\0';
}

// C56A4180-65AA-42EC-A945-5FD21DEC0538
// NWLRBBMQ-BHCD-ARZO-WKKY-HIDDQSCDXRJM