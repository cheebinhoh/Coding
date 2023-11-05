#ifndef HAL_PROC_MANAGER_H_HPP_HAVE_SEEN

#define HAL_PROC_MANAGER_H_HPP_HAVE_SEEN

class Hal_Proc_Manager
{
  public:
      Hal_Proc_Manager(int nr_concurrent = 2);

  private:
      int m_nr_concurrent {};
};

#endif /* HAL_PROC_MANAGER_H_HPP_HAVE_SEEN */
