const should = require('should');
const path = require('path');
const fs = require('fs');
const { runSubmissionAsync, checkSignatureAsync } = require('../');
const cases = require('../test_files/cases.json');

const readTestFile = filename => fs.readFileSync(path.join('test_files', filename), 'utf-8');
const sumReduce = (a, b) => a + b;
const invalidJson = {}; // invalid json with circular object
invalidJson.a = invalidJson;
invalidJson.b = invalidJson;

describe('Test RunSubmissionAsync', () => {
  describe('Failure Tests', () => {
    let signature;
    let input;
    let submissionCode;
    beforeEach(() => {
      signature = JSON.parse(readTestFile('case1_signature.txt'));
      input = JSON.parse(readTestFile('FullParametersData.txt'));
      submissionCode = readTestFile('MockedClass.cpp');
    });

    it('should throw error if signature is not object', () => runSubmissionAsync(1, input, submissionCode)
      .should.be.rejectedWith({ message: 'expected arg 0: object signature' }));

    it('should throw error if input is not object', () => runSubmissionAsync(signature, 1, submissionCode)
      .should.be.rejectedWith({ message: 'expected arg 1: object input' }));

    it('should throw error if submissionCode is not string', () => runSubmissionAsync(signature, input, 1)
      .should.be.rejectedWith({ message: 'expected arg 2: string submissionCode' }));


    it('should throw error if signature is invalid json object', () => runSubmissionAsync(invalidJson, input, submissionCode)
      .should.be.rejectedWith({ message: 'expected arg 0: object signature is valid json object' }));

    it('should throw error if input is invalid json object', () => runSubmissionAsync(signature, invalidJson, submissionCode)
      .should.be.rejectedWith({ message: 'expected arg 1: object input is valid json object' }));

    ['input', 'output', 'className', 'method'].forEach((prop) => {
      it(`should throw error if signature is missing ${prop} key`, async () => {
        delete signature[prop];
        return runSubmissionAsync(signature, input, submissionCode)
          .should.be.rejectedWith({ message: `signature is lack of '${prop}' key` });
      });
      it(`should throw error if ${prop} of signature is invalid`, async () => {
        signature[prop] = 1;
        return runSubmissionAsync(signature, input, submissionCode)
          .should.be.rejectedWith({ message: 'incompatible json type, details: 1' });
      });
    });
    it('should throw error if input of signature contains not string value', async () => {
      signature.input[0] = 1;
      return runSubmissionAsync(signature, input, submissionCode)
        .should.be.rejectedWith({ message: 'incompatible json type, details: 1' });
    });
    it('should throw error if input of signature contains empty string value', async () => {
      signature.input[0] = '';
      return runSubmissionAsync(signature, input, submissionCode)
        .should.be.rejectedWith({ message: 'string value must not be empty under strict mode' });
    });

    it('should throw error if input of signature contains invalid string value', async () => {
      signature.input[0] = 'wrong';
      return runSubmissionAsync(signature, input, submissionCode)
        .should.be.rejectedWith({ message: 'input value type <wrong> is not accepted' });
    });

    ['output', 'className', 'method'].forEach((prop) => {
      it(`should throw error if ${prop} of signature have empty string value`, async () => {
        signature[prop] = '';
        return runSubmissionAsync(signature, input, submissionCode)
          .should.be.rejectedWith({ message: 'string value must not be empty under strict mode' });
      });
    });
    it('should throw error if output of signature contains invalid string value', async () => {
      signature.output = 'wrong';
      return runSubmissionAsync(signature, input, submissionCode)
        .should.be.rejectedWith({ message: 'output value type <wrong> is not accepted' });
    });

    it('should throw error if input is invalid', async () => {
      input = {};
      return runSubmissionAsync(signature, input, submissionCode)
        .should.be.rejectedWith({ message: 'incompatible json type, details: {}' });
    });

    it('should throw error if input contains input item', async () => {
      input = [{}];
      return runSubmissionAsync(signature, input, submissionCode)
        .should.be.rejectedWith({ message: 'inputItem is lack of \'input\' key' });
    });

    it('should throw error if inputItem of input is invalid type', async () => {
      input = [{ input: 1 }];
      return runSubmissionAsync(signature, input, submissionCode)
        .should.be.rejectedWith({ message: 'incompatible json type, details: 1' });
    });

    it('should throw error if input is missing data', async () => {
      input = [{ input: [] }];
      return runSubmissionAsync(signature, input, submissionCode)
        .should.be.rejectedWith({ message: 'please check the input size of []' });
    });

    it('should throw error if inputItem of input contains invalid int type', async () => {
      input = [{ input: Array.from(new Array(signature.input.length), (x, i) => `wrong${i}`) }];
      return runSubmissionAsync(signature, input, submissionCode)
        .should.be.rejectedWith({ message: 'incompatible int parameter_0: wrong0' });
    });

    it('should throw error if inputItem of input contains invalid double type', async () => {
      input = [{ input: Array.from(new Array(signature.input.length), (x, i) => (i === 1 ? 'wrong' : i)) }];
      return runSubmissionAsync(signature, input, submissionCode)
        .should.be.rejectedWith({ message: 'incompatible double parameter_1: wrong' });
    });

    it('should throw error if inputItem of input contains invalid string type', async () => {
      input = [{ input: Array.from(new Array(signature.input.length), (x, i) => i) }];
      return runSubmissionAsync(signature, input, submissionCode)
        .should.be.rejectedWith({ message: 'incompatible string parameter_2: 2' });
    });

    it('should throw error if inputItem of input contains invalid vector<int>', async () => {
      input[0].input[3] = ['wrongint'];
      return runSubmissionAsync(signature, input, submissionCode)
        .should.be.rejectedWith({ message: 'incompatible json type, details: "wrongint"' });
    });

    it('should throw error if inputItem of input contains invalid vector<double>', async () => {
      input[0].input[4] = ['wrongdouble'];
      return runSubmissionAsync(signature, input, submissionCode)
        .should.be.rejectedWith({ message: 'incompatible json type, details: "wrongdouble"' });
    });

    it('should throw error if inputItem of input contains invalid vector<string>', async () => {
      input[0].input[5][0] = 1;
      return runSubmissionAsync(signature, input, submissionCode)
        .should.be.rejectedWith({ message: 'incompatible vector<string> parameter_5: 1' });
    });
    it('should throw error if inputItem of input contains invalid vector<string>', async () => {
      input[0].input[5][0] = 1;
      return runSubmissionAsync(signature, input, submissionCode)
        .should.be.rejectedWith({ message: 'incompatible vector<string> parameter_5: 1' });
    });
  });
  it('currently fail to run submission code with void return type', async () => {
    // test void return type function without arguments.
    // currently have issue to run with void return type
    const res = await runSubmissionAsync({
      input: [],
      output: 'void',
      className: 'Test',
      method: 'test1',
    }, [{ input: [] }], 'class Test{public: void test1();};');
    should.exist(res.results);
    res.results[0].should.have.property('error', 'Runtime error: variable has incomplete type \'void\'\n');
  });

  describe('Cases Tests', () => {
    cases.forEach((sample, index) => {
      const testIndex = index + 1;
      const signature = JSON.parse(readTestFile(`case${testIndex}_signature.txt`));
      const inputData = JSON.parse(readTestFile(sample[1]));
      const submissionCode = readTestFile(sample[0]);
      if (testIndex !== 6) {
        it(`should run Test case ${testIndex} ${sample[2]} without error`, async () => {
          const res = await runSubmissionAsync(signature, inputData, submissionCode);
          should.exist(res);
          const results = JSON.parse(readTestFile(`case${testIndex}_result.txt`));
          res.should.match(results);
          if (!results.error) {
            const statistics = res.results
              .filter(r => !r.error && r.statistics
                  && !Number.isNaN(r.statistics.memory) && !Number.isNaN(r.statistics.time))
              .map(x => x.statistics);
            ['memory', 'time'].forEach((prop) => {
              const propArray = statistics.map(x => x[prop]);
              res.statistics[prop].should.match({
                max: Math.max(...propArray),
                mean: propArray.reduce(sumReduce, 0) / propArray.length,
                min: Math.min(...propArray),
              });
            });
          }
        });
      } else {
        it(`should run Test case 6 ${sample[2]} with error`, async () => runSubmissionAsync(signature, inputData, submissionCode)
          .should.be.rejectedWith({ message: 'please check the input size of [["test","string","array"]]' }));
      }
    });
  });
});

describe('Test checkSignatureAsync', () => {
  describe('Failure Tests', () => {
    let signature;
    let submissionCode;
    beforeEach(() => {
      signature = JSON.parse(readTestFile('case1_signature.txt'));
      submissionCode = readTestFile('MockedClass.cpp');
    });

    it('should throw error if signature is not object', () => checkSignatureAsync(1, submissionCode)
      .should.be.rejectedWith({ message: 'expected arg 0: object signature' }));

    it('should throw error if submissionCode is not string', () => checkSignatureAsync(signature, 1)
      .should.be.rejectedWith({ message: 'expected arg 1: string submissionCode' }));

    it('should throw error if signature is invalid json object', () => checkSignatureAsync(invalidJson, submissionCode)
      .should.be.rejectedWith({ message: 'expected arg 0: object signature is valid json object' }));

    ['input', 'output', 'className', 'method'].forEach((prop) => {
      it(`should throw error if signature is missing ${prop} key`, async () => {
        delete signature[prop];
        return checkSignatureAsync(signature, submissionCode)
          .should.be.rejectedWith({ message: `signature is lack of '${prop}' key` });
      });
      it(`should throw error if ${prop} of signature is invalid`, async () => {
        signature[prop] = 1;
        return checkSignatureAsync(signature, submissionCode)
          .should.be.rejectedWith({ message: 'incompatible json type, details: 1' });
      });
    });
    it('should throw error if input of signature contains not string value', async () => {
      signature.input[0] = 1;
      return checkSignatureAsync(signature, submissionCode)
        .should.be.rejectedWith({ message: 'incompatible json type, details: 1' });
    });
    it('should throw error if input of signature contains empty string value', async () => {
      signature.input[0] = '';
      return checkSignatureAsync(signature, submissionCode)
        .should.be.rejectedWith({ message: 'string value must not be empty under strict mode' });
    });

    it('should throw error if input of signature contains invalid string value', async () => {
      signature.input[0] = 'wrong';
      return checkSignatureAsync(signature, submissionCode)
        .should.be.rejectedWith({ message: 'input value type <wrong> is not accepted' });
    });

    ['output', 'className', 'method'].forEach((prop) => {
      it(`should throw error if ${prop} of signature have empty string value`, async () => {
        signature[prop] = '';
        return checkSignatureAsync(signature, submissionCode)
          .should.be.rejectedWith({ message: 'string value must not be empty under strict mode' });
      });
    });
    it('should throw error if output of signature contains invalid string value', async () => {
      signature.output = 'wrong';
      return checkSignatureAsync(signature, submissionCode)
        .should.be.rejectedWith({ message: 'output value type <wrong> is not accepted' });
    });
  });
  describe('Cases Tests', () => {
    it('should check signature for void return type without error', async () => {
      // test void return type function without arguments.
      const res1 = await checkSignatureAsync({
        input: [],
        output: 'void',
        className: 'Test',
        method: 'test1',
      }, 'class Test{public: void test1();};');
      should.exist(res1);
      should.not.exist(res1.error);
      res1.should.have.property('exist', true);
      // test void return type function with one arguments
      const res2 = await checkSignatureAsync({
        input: ['string'],
        output: 'void',
        className: 'Test',
        method: 'test2',
      }, 'class Test{public: void test2(std::string);};');
      should.exist(res2);
      should.not.exist(res2.error);
      res2.should.have.property('exist', true);
    });
    cases.forEach((sample, index) => {
      const testIndex = index + 1;
      const signature = JSON.parse(readTestFile(`case${testIndex}_signature.txt`));
      const submissionCode = readTestFile(sample[0]);
      it(`should check signature in Test case ${testIndex} ${sample[2]} without error`, async () => {
        const res = await checkSignatureAsync(signature, submissionCode);
        should.exist(res);
        if (testIndex === 2) {
          should.exist(res.error);
          res.exist.should.be.false();
          res.error.should.match(/use of undeclared identifier 'THIS_IS_AN_INVALID_LINE_THAT_WOULD_CAUSE_COMPILATION_ERROR'/);
        } else {
          should.not.exist(res.error);
          res.should.have.property('exist', testIndex !== 6);
          signature.input.push('int');
          // add invalid input type to ensure no error and not exist
          const res2 = await checkSignatureAsync(signature, submissionCode);
          should.exist(res2);
          should.not.exist(res2.error);
          res2.exist.should.be.false();
        }
      });
    });
  });
});
