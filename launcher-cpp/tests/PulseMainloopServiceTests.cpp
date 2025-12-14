//
// Created by bimba on 11/19/25.
//

#include "Tests.hpp"

TEST(PulseMainloopService, issue)
{
    pulse::MainloopService service{TestData::services()};
    ASSERT_TRUE(service.available());
    pa_context* ctx = service.issue(TestData::uuid().c_str());
    ASSERT_NE(nullptr, ctx);
    ASSERT_EQ(PA_CONTEXT_UNCONNECTED, pa_context_get_state(ctx));
    ASSERT_EQ(1, service.getIssued());
    service.unref(ctx);
    ASSERT_EQ(0, service.getIssued());
}
