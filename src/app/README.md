# Transitional app root

T418 moves each project-owned implementation to its owning executable. This
root currently retains only the BaseSrv entry and its Console query helper until
S3 moves them to `src/basesrv/`. It is not a shared application component.
