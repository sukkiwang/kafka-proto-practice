// ts-aim-audt-auditevents-tsspd1-kfns-px289
// ts-aim-audt-auditevents-tsspd1-kfns-px289


#include <librdkafka/rdkafkacpp.h>

#include <string>
#include <stdlib.h>
#include "pnr.pb.h"

#include <vector>
#include <iostream>

using namespace amapnr;
/*
 * Handle and print a consumed message.
 *
 * Internally crafted messages are also used to propagate state from
 * librdkafka to the application. The application needs to check
 * the `rkmessage->err` field for this purpose.
 */
void msg_consume(RdKafka::Message* message, void* opaque) {
  std::string  msg;
  //audt::om::Ticket ti;
  PNR pnr;

  switch (message->err())
  {
    case RdKafka::ERR__TIMED_OUT:
      break;

    case RdKafka::ERR_NO_ERROR:
      if (message->key()) {
        //std::cout << "Key:   " << *message->key() << std::endl;
      }

      // Decode message
      
      msg = std::string(static_cast<char *>(message->payload()));
      
      //       //Ticket
      // if(ti.ParseFromString(msg))
      // {
      //   std::cout << "Ticket id: " << ti.id().ticket_number() << std::endl;
      // }
      //Update 
      if(pnr.ParseFromString(msg) && pnr.rloc().rloc().size() == 6 && !pnr.flightnum().empty())
      {
        std::cout << "PNR rloc: " << pnr.rloc().rloc() << std::endl;
        std::cout << "PNR flight number: " << pnr.flightnum() << std::endl;
      }

      else
      {
        /*printf("Value: %.*s\n\n",
          static_cast<int>(message->len()),
          static_cast<const char *>(message->payload()));*/
      }

      message->topic()->offset_store(message->partition(), message->offset());
      break;

    case RdKafka::ERR__UNKNOWN_TOPIC:
    case RdKafka::ERR__UNKNOWN_PARTITION:
      std::cerr << "Consume failed: " << message->errstr() << std::endl;
      break;

    default:
      /* Errors */
      std::cerr << "Consume failed: " << message->errstr() << std::endl;
  }
}

/**
 * MAIN - Start here
 **/
int main(int argc, char **argv) {
  /* Topic created using the BMS Web UI(https://bms.prod.bloomberg.com) */
  //ts-aim-audt-auditevents-tsspd1-kfns-px289
  std::string consumer_group = "TEST_group";
  if(argc == 2)
  {
    consumer_group = std::string(argv[1]);
  }
  std::string topicName = "rds-dcs-syn-pnr";

  /* Dynamically set the broker endpoint using BMS Discovery
   * See https://bbgithub.dev.bloomberg.com/pages/bms/docs/discovery/intro/ for
   * more information
   */

  std::string brokerList = "127.0.0.1:9092";

  std::cout << "Using Broker list: " << brokerList << std::endl;

  /* Topic Configuration object */
  RdKafka::Conf *topicConf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);

  std::string errstr;
  /* Action to take when there is no initial offset in offset store or
   * the desired offset is out of range:
   * 'smallest','earliest' - automatically reset to smallest offset
   * 'largest','latest'    - automatically reset to largest offset
   */
  topicConf->set("auto.offset.reset", "largest" , errstr);

  /* Global Configuration object */
  RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);

  /* Set default topic config for pattern-matched topics.
   * Note - topicConf is freed by this function and must not be used later
   */
  conf->set("default_topic_conf", topicConf, errstr);
  delete topicConf;

  conf->set("metadata.broker.list", brokerList, errstr);

  /* Set an application identifier */
  conf->set("client.id", "pnr-consumer" , errstr);

  /* Manually store offsets for committing */
  conf->set("enable.auto.offset.store", "false", errstr);

  /* Disable logging of noisy broker disconnects. */
  conf->set("log.connection.close", "false", errstr);

  /* Disable EOF events */
  conf->set("enable.partition.eof", "false", errstr);

  /* Set consumer group - MANDATORY */
  conf->set("group.id", consumer_group, errstr);

  /* Create Consumer Handle */
  RdKafka::KafkaConsumer *consumer=RdKafka::KafkaConsumer::create(conf,errstr);
  if (!consumer) {
    std::cerr << "Failed to create consumer: " << errstr << std::endl;
    exit(1);
  }

  std::cout << "% Created consumer " << consumer->name() << std::endl;

  /* Creating a Topic* so it is available via message->topic() during
   * processing (so offsets can be stored).
   */
  RdKafka::Topic::create(consumer, topicName, NULL, errstr);

  /* Subscribe to topics */
  std::vector<std::string> topics;
  topics.push_back(topicName);
  RdKafka::ErrorCode err = consumer->subscribe(topics);
  if (err) {
    std::cerr << "Failed to subscribe to " << topics.size() << " topics: "
              << RdKafka::err2str(err) << std::endl;
    exit(1);
  }

  std::cout << "Succedd to subscribe, Consumer group : "<< consumer_group << std::endl;
  std::vector< RdKafka::TopicPartition * > partitions;



  
  /* Check for messages for up to 1 minute from the topic-partition list */
  //int i = 1;
  while (true) {
    RdKafka::Message *msg = consumer->consume(1000);

    RdKafka::ErrorCode err_ass = consumer->assignment(partitions);
    if(err_ass)
    {
      std::cerr << "Failed to get assignment "
                << RdKafka::err2str(err) << std::endl;
      exit(1);
    }

    for(auto i : partitions)
    {
      std::cout << "assigned partition : "<< i->partition() << " offset "<< i->offset() << std::endl;
    }
      msg_consume(msg, NULL);
      delete msg;
    }

  /* Stop consumer */
  consumer->close();
  delete consumer;
  delete conf;

  return 0;
}