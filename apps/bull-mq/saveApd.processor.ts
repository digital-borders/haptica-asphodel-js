import { Job } from "bullmq";

const saveApdProcessor = async (job: Job) => {
  const childrenValues = await job.getChildrenValues();

  console.log("saveApdWorker: childrenValues", childrenValues);
  return {
    apd: "apd",
  };
};

export default saveApdProcessor;
