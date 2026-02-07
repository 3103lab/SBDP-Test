// SPDX-License-Identifier: LicenseRef-SBPD-1.0
/******************************************************************************
 * @file    LightTest.h
 * @brief   Light Test Framework
 * @author  Satoh
 * @note    
 * Copyright (c) 2026 Satoh(3103lab.com)
 *****************************************************************************/
#pragma once
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <chrono>
#include <functional>

namespace ltest {

    struct Failure {
        std::string strTestName;
        std::string strFile;
        int64_t     snLine = 0;
        std::string strExpr;
        std::string strMessage;
    };

    struct TestCase {
        std::string           strName;
	    std::function<void()> fnTestFunction;
    };

    struct RunOptions {
        bool bVerbose = true;  // 各テストのOK/NGを出す
    };

    enum class ResultCode : uint8_t {
        Passed = 0,
        Failed,
        Skipped,
    };

    struct TestResult {
        std::string strName;
		ResultCode  eResult = ResultCode::Skipped;
    };

    struct RunStatistics {
        uint64_t unTotal     = 0;
        uint64_t unRun       = 0;
        uint64_t unPassed    = 0;
        uint64_t unFailed    = 0;
        uint64_t unSkipped   = 0;
        uint64_t unElapsedMs = 0;
    };

    class LightTest{
    public:
        static LightTest& Instance() {
            static LightTest s_cLightTest;
            return s_cLightTest;
        }

        void AddTestCase(const std::string& strName, std::function<void()> fnTestFunction) {
            TestCase cTestCase{};
            cTestCase.strName = strName;
            cTestCase.fnTestFunction = fnTestFunction;
            m_vecTestCase.push_back(std::move(cTestCase));
        }
        const std::vector<TestCase>& GetTests() const { return m_vecTestCase; }

        inline bool RunAllTests(const RunOptions& opt = {});
        inline void AddFailure(const std::string& strFile, int snLine, const std::string& strExpr, const std::string& strMsg); 
    private:

    private:
        std::vector<TestCase> m_vecTestCase;
        std::vector<Failure>  m_vecFailure;
        std::string           m_strCurrentTestName;
		uint64_t			  m_unCurrentFailure = 0;
    };

	// inline implementations
    inline bool LightTest::RunAllTests(const RunOptions& opt)
    {
        using clock = std::chrono::steady_clock;
        auto tmBegin = clock::now();

        RunStatistics stStatiscs{};
        stStatiscs.unTotal = static_cast<uint64_t>(m_vecTestCase.size());

        std::vector<TestCase> vecTestResult;

        for (const auto& stTestCase : m_vecTestCase) {

            stStatiscs.unRun++;
            m_strCurrentTestName = stTestCase.strName;
            m_unCurrentFailure   = 0;

            bool bOk = true;
            try {
                stTestCase.fnTestFunction();
            }
            catch (const std::exception& e) {
                AddFailure(__FILE__, __LINE__, "uncathed exception", e.what());
                bOk = false;
            }
            catch (...) {
                AddFailure(__FILE__, __LINE__, "unknown exception", "caught");
                bOk = false;
            }
            if (m_unCurrentFailure > 0) {
				bOk = false;
            }

            if (bOk) {
                stStatiscs.unPassed++;
                if (opt.bVerbose) std::printf("[  OK  ] %s\n", m_strCurrentTestName.c_str());
            }
            else {
                stStatiscs.unFailed++;
                if (opt.bVerbose) std::printf("[ FAIL ] %s\n", m_strCurrentTestName.c_str());
            }
        }

        auto tmEnd = clock::now();
        stStatiscs.unElapsedMs = static_cast<uint64_t>(
            std::chrono::duration_cast<std::chrono::milliseconds>(tmEnd - tmBegin).count()
            );

        std::printf("\n==== test summary ====\n");
        std::printf("Total   : %llu\n",   (unsigned long long)stStatiscs.unTotal);
        std::printf("Run     : %llu\n",   (unsigned long long)stStatiscs.unRun);
        std::printf("Passed  : %llu\n",   (unsigned long long)stStatiscs.unPassed);
        std::printf("Failed  : %llu\n",   (unsigned long long)stStatiscs.unFailed);
        std::printf("Skipped : %llu\n",   (unsigned long long)stStatiscs.unSkipped);
        std::printf("Elapsed : %llums\n", (unsigned long long)stStatiscs.unElapsedMs);

        if (!m_vecFailure.empty()) {
            std::printf("\n-- failures (%zu) --\n", m_vecFailure.size());
            for (const auto& stFailure : m_vecFailure) {
                std::printf("[%s] %s:%ld\n  expr: %s\n  msg : %s\n",
                    stFailure.strTestName.c_str(),
                    stFailure.strFile.c_str(),
                    stFailure.snLine,
                    stFailure.strExpr.c_str(),
                    stFailure.strMessage.c_str()
                );
            }
        }

        return stStatiscs.unFailed == 0;
    }

    inline void LightTest::AddFailure(const std::string& strFile, int snLine, const std::string& strExpr, const std::string& strMsg)
    {
        Failure stFailure{};
        stFailure.strTestName = m_strCurrentTestName;
        stFailure.strFile = strFile;
        stFailure.snLine = snLine;
        stFailure.strExpr = strExpr;
        stFailure.strMessage = strMsg;
        m_vecFailure.push_back(std::move(stFailure));
        m_unCurrentFailure++;
    }

    inline void CheckTrue(bool bValue, const std::string& strFile, int snLine, const std::string& strExpr)
    {
        if (!bValue) {
            LightTest::Instance().AddFailure(strFile, snLine, strExpr, "expected true, but was false");
        }
    }

    inline void CheckFalse(bool bValue, const std::string& strFile, int snLine, const std::string& strExpr)
    {
        if (bValue) {
            LightTest::Instance().AddFailure(strFile, snLine, strExpr, "expected false, but was true");
        }
    }

    template <typename TA_, typename TB_>
    inline void CheckEqual(const TA_& a, const TB_& b,
        const std::string& strFile, int snLine,
        const std::string& strExpr,
        const std::string& strLeft, const std::string& strRight)
    {
        if (!(a == b)) {
            std::string strMsg = "expected equality: " + strLeft + " == " + strRight;
            LightTest::Instance().AddFailure(strFile, snLine, strExpr, strMsg);
        }
    }

    template <typename TA, typename TB>
    inline void CheckNotEqual(const TA& a, const TB& b,
        const std::string& strFile, int snLine,
        const std::string& strExpr,
        const std::string& strLeft, const std::string& strRight)
    {
        if (!(a != b)) {
            std::string strMsg = "expected inequality: " + strLeft + " != " + strRight;
            LightTest::Instance().AddFailure(strFile, snLine, strExpr, strMsg);
        }
    }

    template <typename TA, typename TB>
    inline void CheckLessThan(const TA& a, const TB& b,
        const std::string& strFile, int snLine,
        const std::string& strExpr,
        const std::string& strLeft, const std::string& strRight)
    {
        if (!(a < b)) {
            LightTest::Instance().AddFailure(strFile, snLine, strExpr, "expected: " + strLeft + " < " + strRight);
        }
    }

    template <typename TA, typename TB>
    inline void CheckLessEqual(const TA& a, const TB& b,
        const std::string& strFile, int snLine,
        const std::string& strExpr,
        const std::string& strLeft, const std::string& strRight)
    {
        if (!(a <= b)) {
            LightTest::Instance().AddFailure(strFile, snLine, strExpr, "expected: " + strLeft + " <= " + strRight);
        }
    }

    template <typename TA, typename TB>
    inline void CheckGreaterThan(const TA& a, const TB& b,
        const std::string& strFile, int snLine,
        const std::string& strExpr,
        const std::string& strLeft, const std::string& strRight)
    {
        if (!(a > b)) {
            LightTest::Instance().AddFailure(strFile, snLine, strExpr, "expected: " + strLeft + " > " + strRight);
        }
    }

    template <typename TA, typename TB>
    inline void CheckGreaterEqual(const TA& a, const TB& b,
        const std::string& strFile, int snLine,
        const std::string& strExpr,
        const std::string& strLeft, const std::string& strRight)
    {
        if (!(a >= b)) {
            LightTest::Instance().AddFailure(strFile, snLine, strExpr, "expected: " + strLeft + " >= " + strRight);
        }
    }

    inline void CheckStringEqual(const char* pszA, const char* pszB,
        const std::string& strFile, int snLine,
        const std::string& strExpr)
    {
        const bool bEqual =
            (pszA == nullptr && pszB == nullptr) ||
            (pszA != nullptr && pszB != nullptr && std::strcmp(pszA, pszB) == 0);

        if (!bEqual) {
            LightTest::Instance().AddFailure(strFile, snLine, strExpr, "expected strings to be equal");
        }
    }

    inline void ForceFailure(const std::string& strFile, int snLine,
        const std::string& strExpr,
        const std::string& strMessage)
    {
        LightTest::Instance().AddFailure(strFile, snLine, strExpr, strMessage);
    }


// ============================
// Macros: test entry & asserts
// ============================

#ifndef LTEST_CONCAT_INNER
#define LTEST_CONCAT_INNER(a, b) a##b
#endif
#ifndef LTEST_CONCAT
#define LTEST_CONCAT(a, b) LTEST_CONCAT_INNER(a, b)
#endif

#ifndef LTEST_DEFINE_TEST
#define LTEST_DEFINE_TEST(test_name)                                                   \
    static void test_name();                                                           \
    namespace {                                                                        \
        struct LTEST_CONCAT(_ltest_reg_, test_name) {                                  \
            LTEST_CONCAT(_ltest_reg_, test_name)() {                                   \
                ::ltest::LightTest::Instance().AddTestCase(#test_name, &test_name);    \
            }                                                                          \
        } LTEST_CONCAT(_ltest_reg_instance_, test_name);                               \
    }                                                                                  \
    static void test_name()
#endif


#ifndef LTEST_EXPECT_TRUE
#define LTEST_EXPECT_TRUE(expr)  ::ltest::CheckTrue(expr, __FILE__, __LINE__, #expr)
#endif

#ifndef LTEST_EXPECT_FALSE
#define LTEST_EXPECT_FALSE(expr) ::ltest::CheckFalse(expr, __FILE__, __LINE__, #expr)
#endif

#ifndef LTEST_EXPECT_EQ
#define LTEST_EXPECT_EQ(a, b) \
    do { \
        const auto _ltest_a = (a); \
        const auto _ltest_b = (b); \
        ::ltest::CheckEqual(_ltest_a, _ltest_b, __FILE__, __LINE__, std::string(#a) + " == " + #b, #a, #b); \
    } while (0)
#endif

#ifndef LTEST_EXPECT_NE
#define LTEST_EXPECT_NE(a, b) \
    do { \
        const auto _ltest_a = (a); \
        const auto _ltest_b = (b); \
        ::ltest::CheckNotEqual(_ltest_a, _ltest_b, __FILE__, __LINE__, std::string(#a) + " != " + #b, #a, #b); \
    } while (0)
#endif

#ifndef LTEST_EXPECT_LT
#define LTEST_EXPECT_LT(a, b) \
    do { \
        const auto _ltest_a = (a); \
        const auto _ltest_b = (b); \
        ::ltest::CheckLessThan(_ltest_a, _ltest_b, __FILE__, __LINE__, std::string(#a) + " < " + #b, #a, #b); \
    } while (0)
#endif

#ifndef LTEST_EXPECT_LE
#define LTEST_EXPECT_LE(a, b) \
    do { \
        const auto _ltest_a = (a); \
        const auto _ltest_b = (b); \
        ::ltest::CheckLessEqual(_ltest_a, _ltest_b, __FILE__, __LINE__, std::string(#a) + " <= " + #b, #a, #b); \
    } while (0)
#endif

#ifndef LTEST_EXPECT_GT
#define LTEST_EXPECT_GT(a, b) \
    do { \
        const auto _ltest_a = (a); \
        const auto _ltest_b = (b); \
        ::ltest::CheckGreaterThan(_ltest_a, _ltest_b, __FILE__, __LINE__, std::string(#a) + " > " + #b, #a, #b); \
    } while (0)
#endif

#ifndef LTEST_EXPECT_GE
#define LTEST_EXPECT_GE(a, b) \
    do { \
        const auto _ltest_a = (a); \
        const auto _ltest_b = (b); \
        ::ltest::CheckGreaterEqual(_ltest_a, _ltest_b, __FILE__, __LINE__, std::string(#a) + " >= " + #b, #a, #b); \
    } while (0)
#endif

#ifndef LTEST_EXPECT_STREQ
#define LTEST_EXPECT_STREQ(a, b) \
    ::ltest::CheckStringEqual((a), (b), __FILE__, __LINE__, std::string(#a) + " == " + #b)
#endif

#ifndef LTEST_FAIL
#define LTEST_FAIL(msg) \
    ::ltest::ForceFailure(__FILE__, __LINE__, "LTEST_FAIL", (msg))
#endif

#ifndef LTEST_ASSERT_TRUE
#define LTEST_ASSERT_TRUE(expr) LTEST_EXPECT_TRUE(expr)
#endif
#ifndef LTEST_ASSERT_FALSE
#define LTEST_ASSERT_FALSE(expr) LTEST_EXPECT_FALSE(expr)
#endif
#ifndef LTEST_ASSERT_EQ
#define LTEST_ASSERT_EQ(a, b) LTEST_EXPECT_EQ(a, b)
#endif
#ifndef LTEST_ASSERT_NE
#define LTEST_ASSERT_NE(a, b) LTEST_EXPECT_NE(a, b)
#endif
#ifndef LTEST_ASSERT_LT
#define LTEST_ASSERT_LT(a, b) LTEST_EXPECT_LT(a, b)
#endif
#ifndef LTEST_ASSERT_LE
#define LTEST_ASSERT_LE(a, b) LTEST_EXPECT_LE(a, b)
#endif
#ifndef LTEST_ASSERT_GT
#define LTEST_ASSERT_GT(a, b) LTEST_EXPECT_GT(a, b)
#endif
#ifndef LTEST_ASSERT_GE
#define LTEST_ASSERT_GE(a, b) LTEST_EXPECT_GE(a, b)
#endif
#ifndef LTEST_ASSERT_STREQ
#define LTEST_ASSERT_STREQ(a, b) LTEST_EXPECT_STREQ(a, b)
#endif

}// namespace ltest

