#ifndef TEMPLATE_CLASS_H_HAVE_SEEN

#define TEMPLATE_CLASS_H_HAVE_SEEN

template <typename T>
class SampleTemplate {
    public:
        SampleTemplate(T member) : m_member{member} {
        }

    T& GetMember() {
	return m_member;
    }
 
    const T& GetMember() const;

    private:
        T m_member{};
};

#endif /* TEMPLATE_CLASS_H_HAVE_SEEN */
