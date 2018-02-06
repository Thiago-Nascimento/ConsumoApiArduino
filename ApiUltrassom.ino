# include <ArduinoJson.h>
# include <RestClient.h>
# include <UIPEthernet.h>
# include <Ultrasonic.h>

const byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x21};

void (*reset) () = 0; 

char id[5] = {};

EthernetClient ethclient;

Ultrasonic ultrasonic(9,8);

#define CONTENT_TYPE        "application/json"
#define SERVER              "192.168.2.185"
#define ENDPOINT            "/api/Sensor/"
#define TAMANHO_RESPOSTA    50
#define OFFSET_RESPOSTA     10

RestClient client = RestClient(SERVER, 8080, ethclient);

void setupEthernet() 
{
    if (Ethernet.begin(mac))
    {
        Serial.println("Conectado via DHCP");
        Serial.print("IP recebido:");
        Serial.println(Ethernet.localIP()); 
    }
    else
    {
        Serial.println("Falha DHCP");
    }
}

void enviarMedicao() 
{
    String body = "{\"valor\":";
    body += ultrasonic.distanceRead();
    body += "}";

    Serial.println(body);

    String endpoint = ENDPOINT;
    endpoint += id; 

    char resposta[TAMANHO_RESPOSTA] = {};   // Resposta da API

    int statusCode = client.put(endpoint.c_str(), body.c_str(), resposta, TAMANHO_RESPOSTA);
    
    Serial.print(F("STATUSCODE: "));
    Serial.println(statusCode);

    Serial.print(F("RESPOSTA: "));
    Serial.println(resposta);
    Serial.println("");    
}

void setup() 
{
    Serial.begin(9600);
    setupEthernet();

    client.setContentType(CONTENT_TYPE);

    char resposta[TAMANHO_RESPOSTA] = {};

    Serial.println("Enviando POST...\n");

    int status = client.post(ENDPOINT, "{}", resposta, TAMANHO_RESPOSTA);   // Obter id novo via put, substituir 'resposta' e 'TAMANHO_RESPOSTA' por 0 ou NULL, para ignorar a resposta da API

    if (status != 200)
    {
      Serial.println("Reiniciando...\n");
      Serial.flush();
      reset();
    }

    for (int i = 0; i < 4; i++)
    {
      id[i] = resposta[OFFSET_RESPOSTA + i];
    }
    
    Serial.print("Status: ");
    Serial.println(status);
    Serial.print("Resposta: ");
    Serial.println(resposta);

    Serial.print("ID Obtido: ");
    Serial.println(id);
    Serial.println("");    
}

void loop()
{
  enviarMedicao();
  delay(1000);
}
