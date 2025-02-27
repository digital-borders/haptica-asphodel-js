import { Job } from "bullmq";

const saveProcessedProcessor = async (job: Job) => {
  const childrenValues = await job.getChildrenValues();

  console.log("saveProcessedWorker: childrenValues", childrenValues);
  return {
    processed: "processed",
  };
};

export default saveProcessedProcessor;
