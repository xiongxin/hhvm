/*
   +----------------------------------------------------------------------+
   | HipHop for PHP                                                       |
   +----------------------------------------------------------------------+
   | Copyright (c) 2010-present Facebook, Inc. (http://www.facebook.com)  |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
*/

#include "hphp/runtime/vm/jit/decref-profile.h"

#include "hphp/runtime/base/tv-refcount.h"

namespace HPHP { namespace jit {

///////////////////////////////////////////////////////////////////////////////

void DecRefProfile::reduce(DecRefProfile& a, const DecRefProfile& b) {
  auto const total = static_cast<uint64_t>(a.total + b.total);
  auto constexpr limit = std::numeric_limits<decltype(a.total)>::max();
  if (total > limit) {
    auto scale = [&] (uint32_t& x, uint64_t y) {
      x = ((x + y) * limit + total - 1) / total;
    };
    a.total = limit;
    scale(a.refcounted, b.refcounted);
    scale(a.released, b.released);
    scale(a.decremented, b.decremented);
  } else {
    a.total       = total;
    a.refcounted  += b.refcounted;
    a.released    += b.released;
    a.decremented += b.decremented;
  }
  a.type |= b.type;
}

void DecRefProfile::update(TypedValue tv) {
  auto constexpr max = std::numeric_limits<decltype(total)>::max();
  if (total == max) return;

  total++;
  type |= typeFromTV(&tv, nullptr);
  if (!isRefcountedType(tv.type())) return;
  refcounted++;
  auto const countable = tv.val().pcnt;
  if (countable->decWillRelease()) {
    released++;
  } else if (countable->isRefCounted()) {
    decremented++;
  }
}

void DecRefProfile::updateAndDecRef(TypedValue tv) {
  update(tv);
  tvDecRefGen(tv);
}

folly::dynamic DecRefProfile::toDynamic() const {
  return folly::dynamic::object("total", total)
                               ("uncounted", uncounted())
                               ("percentUncounted", percent(uncounted()))
                               ("persistent", persistent())
                               ("percentPersistent", percent(persistent()))
                               ("destroyed", destroyed())
                               ("percentDestroyed", percent(destroyed()))
                               ("survived", survived())
                               ("percentSurvived", percent(survived()))
                               ("typeStr", type.toString())
                               ("profileType", "DecRefProfile");
}

std::string DecRefProfile::toString() const {
  return folly::sformat(
    "total: {:4}\n uncounted: {:4} ({:.1f}%),\n persistent: {:4} ({:.1f}%),\n"
    " destroyed: {:4} ({:.1f}%),\n survived: {:4} ({:.1f}%),\n typeStr: {}",
    total,
    uncounted(),  percent(uncounted()),
    persistent(), percent(persistent()),
    destroyed(),  percent(destroyed()),
    survived(),   percent(survived()),
    type.toString()
  );
}

const StringData* decRefProfileKey(int locId) {
  return makeStaticString(folly::to<std::string>("DecRefProfile-", locId));
}

const StringData* decRefProfileKey(const IRInstruction* inst) {
  auto const local = inst->extra<DecRefData>()->locId;
  return decRefProfileKey(local);
}

TargetProfile<DecRefProfile> decRefProfile(
    const TransContext& context, const IRInstruction* inst) {
  auto const profileKey = decRefProfileKey(inst);
  return TargetProfile<DecRefProfile>(context, inst->marker(), profileKey);
}

TargetProfile<DecRefProfile> decRefProfile(const TransContext& context,
                                           const BCMarker& marker,
                                           int locId) {
  auto const profileKey = decRefProfileKey(locId);
  return TargetProfile<DecRefProfile>(context, marker, profileKey);
}

///////////////////////////////////////////////////////////////////////////////

}}
