# Real Winmine and CBT hook in one worker

The existing Winmine gameplay runner has a new `-CbtSibling` mode that requires
the hash-identified prebuilt CBT guest. While Winmine's window remained live,
the guest registered its CBT hook, performed nested creation, a veto and later
recovery, then unhooked and exited. The runner observed two distinct task
retirements in the same worker, and Winmine subsequently passed flag, reset,
open-cell, window close and launcher completion checks. All ten staged image
hashes match the published candidate. The guest was not rebuilt or modified.

The existing 16-round common-dialog sibling mode also passed as a regression
control. Its result is retained separately. These tests exercise real guest
windows and a real registered guest hook in a shared worker; they do not force
the exact cross-thread capture/retirement interleaving or prove the old 1400's
cause. They complement the controlled overlap fixture and preserve the prior
[dialog/CBT scheduling non-pass](../dialog-cbt-order-nonpass/review.md).
