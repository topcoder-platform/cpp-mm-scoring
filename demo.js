const path = require('path');
const fs = require('fs');
const cases = require('./test_files/cases.json');
const { runSubmissionAsync } = require('.');

const args = process.argv.slice(2);
let index = 0;
if (args.length === 0 || Number.isNaN(args[0]) || Number(args[0]) <= 0) {
  // use random value if input is invalid.
  index = parseInt(Math.random() * cases.length, 10);
  console.error(`input arguments is invalid and using random index: ${index}`); // eslint-disable-line no-console
} else {
  // input test case number directly
  index = (parseInt(args[0], 10) - 1) % cases.length;
}
const readTestFile = filename => fs.readFileSync(path.join('test_files', filename), 'utf-8');
const signature = JSON.parse(readTestFile(`case${index + 1}_signature.txt`));
const sample = cases[index];
const inputData = JSON.parse(readTestFile(sample[1]));
const submissionCode = readTestFile(sample[0]);
/* eslint-disable no-console */
(async () => {
  try {
    console.log(`Run Test case ${index + 1} to ${sample[2]}`);
    console.log(`Signature: case${index + 1}_signature.txt`);
    console.log(`Input data: ${sample[1]}`);
    console.log(`Test code: ${sample[0]}`);
    const results = await runSubmissionAsync(signature, inputData, submissionCode);
    console.log('Here is the output json:');
    console.log(JSON.stringify(results, null, 4));
  } catch (e) {
    console.error(`Error happened with message: ${e.message}`);
  }
})();
/* eslint-enable no-console */
