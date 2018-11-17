const os = require('os');
const Promise = require('bluebird');
const addon = require('bindings')('MarathonMatchScoring');

if (!process.env || (os.platform() === 'linux' && (!process.env.LD_PRELOAD || !process.env.LD_PRELOAD.includes('libcling')))
    || (os.platform() === 'darwin' && (!process.env.DYLD_INSERT_LIBRARIES || !process.env.DYLD_INSERT_LIBRARIES.includes('libcling')))) {
  console.error('please prepare library libcling in environment variable LD_PRELOAD under linux or DYLD_INSERT_LIBRARIES under osx!'); // eslint-disable-line no-console
  process.exit(1);
}


module.exports = {
  /**
   * Check signature with cling and boost tti
   * @param signature the method signature
   * @param submissionCode the submission code
   */
  checkSignatureAsync: (signature, submissionCode) => new Promise((resolve, reject) => {
    try {
      return addon.checkSignatureAsync(signature, submissionCode, (err, result) => {
        if (err) {
          return typeof err === 'string' ? reject(new Error(err)) : reject(err);
        }
        return resolve(result);
      });
    } catch (e) {
      return reject(e);
    }
  }),
  /**
   * Run submission code with cling
   * @param signature the method signature
   * @param input the method input
   * @param submissionCode the submission code
   */
  runSubmissionAsync: (signature, input, submissionCode) => new Promise((resolve, reject) => {
    try {
      return addon.runSubmissionAsync(signature, input, submissionCode, (err, result) => {
        if (err) {
          return typeof err === 'string' ? reject(new Error(err)) : reject(err);
        }
        return resolve(result);
      });
    } catch (e) {
      return reject(e);
    }
  }),
};
