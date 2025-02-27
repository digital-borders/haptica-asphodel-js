import { FlowProducer } from "bullmq";
import IORedis from "ioredis";
import { Job, Queue, Worker } from "bullmq";

const flowConfig = {};

async function main() {
  const connection = new IORedis({ maxRetriesPerRequest: null });
  // Empty redis
  await connection.flushall();

  // A FlowProducer constructor takes an optional "connection"
  // object otherwise it connects to a local redis instance.
  const flowProducer = new FlowProducer();

  const flow = await flowProducer.add({
    name: "send-mqtt-message",
    queueName: "send-mqtt-message",
    children: [
      {
        name: "save-apd",
        data: { active: true },
        queueName: "save-apd",
        children: [
          {
            name: "save-processed",
            data: { active: true },
            queueName: "save-processed",
            children: [
              {
                name: "calculate",
                data: { active: true },
                queueName: "calculate",
                children: [
                  {
                    name: "acquire-data",
                    data: { active: true },
                    queueName: "acquire-data",
                    children: [],
                  },
                ],
              },
            ],
          },
        ],
      },
    ],
  });

  console.log(flow);

  const baseProcessorPath = `${process.cwd()}/build/apps/bull-mq`;
  const workers: Worker[] = [];
  for (const workerInfo of [
    {
      queueName: "send-mqtt-message",
      workerName: "sendMqttMessage.processor.js",
    },
    { queueName: "save-apd", workerName: "saveApd.processor.js" },
    { queueName: "save-processed", workerName: "saveProcessed.processor.js" },
    { queueName: "calculate", workerName: "calculate.processor.js" },
    { queueName: "acquire-data", workerName: "acquireData.processor.js" },
  ]) {
    const worker = new Worker(
      workerInfo.queueName,
      `${baseProcessorPath}/${workerInfo.workerName}`,
      {
        connection,
      }
    );

    worker.on("stalled", (jobId) => {
      console.log(`[[[ job [id ${jobId}] failed ]]]`);
    });

    worker.on("completed", (job) => {
      console.log(`[[[ job ${job.name} completed ]]]`);
    });

    worker.on("failed", (job) => {
      if (job) {
        console.log(`[[[ job ${job.name} failed return ${job.returnvalue}]]]`);
      }
    });
    workers.push(worker);
  }
}

main()
  .then(() => {
    console.log("done");
  })
  .catch((err) => {
    console.error("MAIN ERROR", err);
  });
