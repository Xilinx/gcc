! { dg-do compile }
! { dg-options "-fcoarray=single -fdump-tree-original" }
!
! Coarray support -- allocatable array coarrays
! PR fortran/18918
!
integer,allocatable :: a(:)[:,:]
nn = 5
mm = 7
allocate(a(nn)[mm,*])
end

subroutine testAlloc3
  implicit none
  integer, allocatable :: ab(:,:,:)[:,:]
  integer, allocatable, dimension(:),codimension[:] :: b(:,:,:)[:,:]
  integer, allocatable, dimension(:,:),codimension[:,:,:] :: c
  integer, allocatable, dimension(:,:),codimension[:,:,:] :: d[:,:]
  integer, allocatable, dimension(:,:,:),codimension[:,:,:] :: e(:,:)
  integer, allocatable, dimension(:,:,:),codimension[:,:,:] :: f(:,:)[:,:]

  allocate(ab(1,2,3)[4,*])
  allocate(b(1,2,3)[4,*])
  allocate(c(1,2)[3,4,*])
  allocate(d(1,2)[3,*])
  allocate(e(1,2)[3,4,*])
  allocate(f(1,2)[3,*])
end subroutine testAlloc3

subroutine testAlloc4()
  implicit none
  integer, allocatable :: xxx(:)[:,:,:,:]
  integer :: mmm
  mmm=88
  allocate(xxx(1)[7,-5:8,mmm:2,*])
end subroutine testAlloc4

subroutine testAlloc5()
  implicit none
  integer, allocatable :: yyy(:)[:,:,:,:]
  integer :: ooo, ppp
  ooo=88
  ppp=42
  allocate(yyy(1)[7,-5:ppp,1,ooo:*])
end subroutine testAlloc5


! { dg-final { scan-tree-dump-times "a.dim.0..lower_bound = 1;"     1 "original" } }
! { dg-final { scan-tree-dump-times "a.dim.0..extent = .*nn - a.dim.0..lower_bound. \\+ 1;" 1 "original" } }
! { dg-final { scan-tree-dump-times "a.dim.1..lower_bound = 1;"     1 "original" } }
! { dg-final { scan-tree-dump-times "a.dim.1..extent = .*mm - a.dim.1..lower_bound. \\+ 1;" 1 "original" } }
! { dg-final { scan-tree-dump-times "a.dim.2..lower_bound = 1;"     1 "original" } }
! { dg-final { scan-tree-dump-times "a.dim.2..extent"          0 "original" } }

! { dg-final { scan-tree-dump-times "xxx.dim.0..lower_bound = 1;"     1 "original" } }
! { dg-final { scan-tree-dump-times "xxx.dim.0..extent = 2 - xxx.dim.0..lower_bound;"     1 "original" } }
! { dg-final { scan-tree-dump-times "xxx.dim.1..lower_bound = 1;"     1 "original" } }
! { dg-final { scan-tree-dump-times "xxx.dim.1..extent = 8 - xxx.dim.1..lower_bound;"     1 "original" } }
! { dg-final { scan-tree-dump-times "xxx.dim.2..lower_bound = -5;"    1 "original" } }
! { dg-final { scan-tree-dump-times "xxx.dim.2..extent = 9 - xxx.dim.2..lower_bound;"     1 "original" } }
! { dg-final { scan-tree-dump-times "xxx.dim.3..lower_bound = .*mmm;" 1 "original" } }
! { dg-final { scan-tree-dump-times "xxx.dim.3..extent = 3 - xxx.dim.3..lower_bound;"     1 "original" } }
! { dg-final { scan-tree-dump-times "xxx.dim.4..lower_bound = 1;"     1 "original" } }
! { dg-final { scan-tree-dump-times "xxx.dim.4..extent"          0 "original" } }

! { dg-final { scan-tree-dump-times "yyy.dim.0..lower_bound = 1;"     1 "original" } }
! { dg-final { scan-tree-dump-times "yyy.dim.0..extent = 2 - yyy.dim.0..lower_bound;"     1 "original" } }
! { dg-final { scan-tree-dump-times "yyy.dim.1..lower_bound = 1;"     1 "original" } }
! { dg-final { scan-tree-dump-times "yyy.dim.1..extent = 8 - yyy.dim.1..lower_bound;"     1 "original" } }
! { dg-final { scan-tree-dump-times "yyy.dim.2..lower_bound = -5;"    1 "original" } }
! { dg-final { scan-tree-dump-times "yyy.dim.2..extent = .*ppp - yyy.dim.2..lower_bound. \\+ 1;" 1 "original" } }
! { dg-final { scan-tree-dump-times "yyy.dim.3..lower_bound = 1;"     1 "original" } }
! { dg-final { scan-tree-dump-times "yyy.dim.3..extent = 2 - yyy.dim.3..lower_bound;"     1 "original" } }
! { dg-final { scan-tree-dump-times "yyy.dim.4..lower_bound = .*ooo;" 1 "original" } }
! { dg-final { scan-tree-dump-times "yyy.dim.4..extent"          0 "original" } }

! { dg-final { cleanup-tree-dump "original" } }
