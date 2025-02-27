import { Job } from "bullmq";

const calculateProcessor = async (job: Job) => {
  const childrenValues = await job.getChildrenValues();

  console.log("calculate.proc.ts: childrenValues", childrenValues);
  return {
    calculatedValue: 5000,
  };
};

export default calculateProcessor;
