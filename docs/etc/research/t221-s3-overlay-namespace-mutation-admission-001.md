# T221 S3: Overlay Namespace Mutation Admission

The next coherent DEM namespace mutation subfamily is `50:04/05/06/17`:
directory create, file delete, directory remove and rename. Its original
owners are `demdir.c` (`demCreateDir`, `demDeleteDir`) and `demfile.c`
(`demDelete`, `demRename`).

Overlay must not invoke those host mutations. Admission requires one private
namespace model supporting directory entries, file/directory tombstones,
same-volume rename, non-empty-directory failure and source-shaped errors. The
current COW file store is insufficient: it has file/tombstone state but no
directory or rename transaction owner.

No BOP route changed. The following implementation must recover all four
identities together, with Direct/Readonly/Overlay/Virtual dispositions and
family regression; trace remains post-family integration evidence only.
