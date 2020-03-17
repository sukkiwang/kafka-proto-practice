//kafka_producer.cpp


#include <librdkafka/rdkafkacpp.h>

#include <string>
#include <stdlib.h>
#include "pnr.pb.h"

#include <vector>
#include <iostream>

//#include <bsl_sstream.h>
//#include <bmsdiscovery_resolve.h>

using namespace amapnr;
using namespace std;
/* Call back function for every message delivery
 *
 * This callback is called exactly once per message, indicating if
 * the message was succesfully delivered
 * (rkmessage->err == RD_KAFKA_RESP_ERR_NO_ERROR) or permanently
 * failed delivery (rkmessage->err != RD_KAFKA_RESP_ERR_NO_ERROR).
 *
 * The callback is triggered from poll() and executes on
 * the application's thread.
 */
class ProducerDeliveryReportCb : public RdKafka::DeliveryReportCb {
 public:
  void dr_cb (RdKafka::Message &message) {
    std::cout << "Message delivery for (" << message.len() << " bytes): " <<
        message.errstr() << std::endl;

    if (message.key()) {
      std::cout << "Key: " << *(message.key()) << ";" << std::endl;
    }
  }
};

void gen_random_rloc(char *s, const int len) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    for (int i = 0; i < len; ++i) {
        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    s[len] = 0;
}

void gen_random_flightNb(char *s, const int len) {
    static const char alpha[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    
    static const char num[] = "0123456789";

    for (int i = 0; i < 2; ++i) {
        s[i] = alpha[rand() % (sizeof(alpha) - 1)];
    }

    for (int i = 2; i < len; ++i) {
        s[i] = num[rand() % (sizeof(num) - 1)];
    }


    s[len] = 0;
}

void PromptForPNR(PNR* aPnr) {


  //cout << "Enter the Rloc number: ";
  char * rand_rloc = new char[7];
  gen_random_rloc(rand_rloc, 6);
  string * rloc = new string(rand_rloc);


  PNR::Rloc *  aRloc = new PNR::Rloc();
  aRloc->set_allocated_rloc(rloc);

  aPnr->set_allocated_rloc(aRloc);

  //cout << "Enter flight number: ";
  char * rand_flightmb = new char[6];
  gen_random_flightNb(rand_flightmb, 5);
  string * flightNum = new string(rand_flightmb);
  //cin >> *flightNum;

  aPnr->set_allocated_flightnum(flightNum);

  //cout << "Enter departure date: ";

}


/**
 * MAIN - Start here
 **/
int main(int argc, char **argv) {
  /* Topic created using the BMS Web UI(https://bms.prod.bloomberg.com) */
  std::string topicName = "rds-dcs-syn-pnr";

  /* Dynamically set the broker endpoint using BMS Discovery
   * See https://bbgithub.dev.bloomberg.com/pages/bms/docs/discovery/intro/ for
   * more information
   */
  std::string brokerList = "127.0.0.1:9092";

  std::cout << "Using Broker list: " << brokerList << std::endl;

  /* RD_KAFKA_PARTITION_UA represents an Unassigned Partition.
   *
   * By default, librdkafka will partition messages using a consistent hash
   * of the key. If no key is provided, messages are randomly partitioned.
   */
  int32_t partition = RdKafka::Topic::PARTITION_UA;

  /* Topic Configuration object */
  RdKafka::Conf *topicConf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);

  std::string errstr;

  /* This field indicates how many acknowledgements the leader broker must
   *  receive from ISR brokers before responding to the request:
   *  0 = Broker does not send any response/ack to client
   *  1 = Only the leader broker will need to ack the message
   * -1 = broker will block until message is committed by all in sync replicas
   *       before sending response.
   */
  topicConf->set("request.required.acks", "-1", errstr);

  /* Global Configuration Object */
  RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);

  /* Set an application identifier */
  conf->set("client.id", "pnr-producer" , errstr);

  /* Set Brokerlist */
  conf->set("metadata.broker.list", brokerList, errstr);

  /* Disable logging of noisy broker disconnects. */
  conf->set("log.connection.close", "false", errstr);

  /* Delay in milliseconds to wait for messages in the producer queue to
   * accumulate before constructing message batches (MessageSets)
   * to transmit to brokers.
   *
   * A higher value allows larger and more effective (less overhead,
   * improved compression) batches of messages to accumulate at the
   * expense of increased message delivery latency
   */
  conf->set("queue.buffering.max.ms", "100" , errstr);

  /* How long to backoff before retrying protocol requests. */
  conf->set("retry.backoff.ms", "500", errstr);

  /* How many times to automatically retry sending a failing MessageSet. */
  conf->set("retries", "10", errstr);

  /* Set compression type to `zstd` */
  conf->set("compression.codec", "zstd", errstr);
  
  /* Set up a message delivery report callback.
   * It will be called once for each message, either on successful
   *  delivery to broker, or upon failure to deliver to broker.
   */
  ProducerDeliveryReportCb drCb;
  conf->set("dr_cb", &drCb, errstr);

  /* Create Producer Instance handle */
  RdKafka::Producer *producer = RdKafka::Producer::create(conf, errstr);
  if (!producer) {
    std::cerr << "Failed to create producer: " << errstr << std::endl;
    exit(1);
  }

  std::cout << "% Created producer " << producer->name() << std::endl;

  /* Create Topic handle */
  RdKafka::Topic *topic = RdKafka::Topic::create(producer, topicName,
                            topicConf, errstr);
  if (!topic) {
    std::cerr << "Failed to create topic: " << errstr << std::endl;
    exit(1);
  }

  /* Produce 100 messages */
  std::string buf;
  std::string key;
  //std::string output;
  cout << "How many pax to be created? " << endl;
  int pax;
  cin >> pax;

  while(pax-- >0)
  {
    PNR * aPnr = new PNR();
    PromptForPNR(aPnr);


    {
      // Write the new address book back to disk.
      //fstream output(argv[1], ios::out | ios::trunc | ios::binary);
      
      if (!aPnr->SerializeToString(&buf)) {
        cerr << "Failed to write address book." << endl;
        return -1;
      }
    }

      //buf="Kafka message: " + s.str();

      /*
      * A key is required if using compaction otherwise it can
      * be set to null.
      */
      key=aPnr->rloc().rloc();

      /*
      * Send/Produce message.
      *
      * This is an asynchronous call, on success it will only
      * enqueue the message on the internal producer queue.
      * The actual delivery attempts to the broker are handled
      * by background threads.
      *
      * The previously registered delivery report callback
      * (msg_delivery_cb) is used to signal back to the application
      * when the message has been delivered (or failed).
      */
      RdKafka::ErrorCode resp = producer->produce(topic, partition,
                                      RdKafka::Producer::RK_MSG_COPY,
                                      const_cast<char*>(buf.c_str()),
                                      buf.size(),
                                      &key, NULL);
      if (resp != RdKafka::ERR_NO_ERROR)
        std::cerr << "% Produce failed: " <<
          RdKafka::err2str(resp) << std::endl;
      else
        std::cerr << "% Produced message (" << buf.size() << " bytes)" <<
          std::endl;

      delete aPnr;
  }



    /* A producer application should continually serve
     * the delivery report queue by calling poll()
     * at frequent intervals.
     * Either put the poll call in your main loop, or in a
     * dedicated thread, or call it after every
     * rd_kafka_produce() call.
     * Just make sure that poll() is still called
     * during periods where you are not producing any messages
     * to make sure previously produced messages have their
     * delivery report callback served (and any other callbacks
     * you register).
     */
    producer->poll(0 /* non-blocking */);

  /* Termination Sequence */

  /* Flush all outstanding requests */
  producer->flush(1000 * 5 /* wait for 5 seconds */);

  /* Destroy configs */
  delete conf;

  /* Destroy topic */
  delete topic;
  delete topicConf;

  /* Destroy producer */
  delete producer;


  return 0;
}
