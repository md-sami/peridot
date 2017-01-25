// Copyright 2017 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "apps/modular/lib/fidl/single_service_app.h"
#include "apps/modular/lib/testing/reporting.h"
#include "apps/modular/lib/testing/testing.h"
#include "apps/modular/services/story/module.fidl.h"
#include "apps/mozart/services/views/view_token.fidl.h"
#include "lib/mtl/tasks/message_loop.h"

using modular::testing::TestPoint;

namespace {

class ChildApp : public modular::SingleServiceApp<modular::Module> {
 public:
  ChildApp() {
    modular::testing::Init(application_context());
  }

  ~ChildApp() override = default;

 private:
  // |Module|
  void Initialize(
      fidl::InterfaceHandle<modular::Story> story,
      fidl::InterfaceHandle<modular::Link> link,
      fidl::InterfaceHandle<modular::ServiceProvider> incoming_services,
      fidl::InterfaceRequest<modular::ServiceProvider> outgoing_services)
      override {
    story_.Bind(std::move(story));
    link_.Bind(std::move(link));
    initialized_.Pass();
  }

  // |Module|
  void Stop(const StopCallback& done) override {
    mtl::MessageLoop::GetCurrent()->PostQuitTask();
    stopped_.Pass();
    modular::testing::Done();
    done();
  }

  modular::StoryPtr story_;
  modular::LinkPtr link_;

  TestPoint initialized_{"Child module initialized"};
  TestPoint stopped_{"Child module stopped"};
};

}  // namespace

int main(int argc, const char** argv) {
  mtl::MessageLoop loop;
  ChildApp app;
  loop.Run();
  // TODO(vardhan): There is a race between module termination killing this
  // process, and this process completing itself, so this may not actually
  // print.
  TEST_PASS("Child module exited normally");
  return 0;
}
