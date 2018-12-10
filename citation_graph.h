//
// Created by kacper on 10.12.18.
//

#ifndef JNP1_5_CITATION_GRAPH_H
#define JNP1_5_CITATION_GRAPH_H

#include <vector>
#include <exception>

class PublicationAlreadyCreated : std::exception
{
    const char * what() const noexcept
    {
        return "PublicationAlreadyCreated";
    }
};

class PublicationNotFound : std::exception
{
    const char * what() const noexcept
    {
        return "PublicationNotFound";
    }
};

class TriedToRemoveRoot : std::exception
{
    const char * what() const noexcept
    {
        return "TriedToRemoveRoot";
    }
};

template <class Publication>
class CitationGraph
{
public:
    CitationGraph(Publication::id_type const &stem_id) noexcept;
    CitationGraph(CitationGraph<Publication> &&other);
    CitationGraph(CitationGraph<Publication> &other) = delete;
    ~CitationGraph();

    CitationGraph<Publication>& operator=(CitationGraph<Publication> &other) = delete;
    CitationGraph<Publication>& operator=(CitationGraph<Publication> &&other) noexcept;
    Publication& operator[](Publication::id_type const &id) const;

    Publication::id_type get_root_id() const noexcept(noexcept(Publication::get_id()));
    std::vector<Publication::id_type> get_children(Publication::id_type const &id) const;
    std::vector<Publication::id_type> get_parents(Publication::id_type const &id) const;
    bool exists(Publication::id_type const &id) const;
    void create(Publication::id_type const &id, Publication::id_type const &parent_id);
    void create(Publication::id_type const &id, std::vector<Publication::id_type> const &parent_ids);
    void add_citation(Publication::id_type const &child_id, Publication::id_type const &parent_id);
    void remove(Publication::id_type const &id);

private:
    class Node
    {
    private:
        Publication::id_type stem_id;
        std::vector<Node *> citations;
    public:
        Node(Publication::id_type stem_id): stem_id(stem_id) {}

        void add_node(Node * another) { citations.emplace_back(another); }
        Publication::id_type getStem_id() const { return stem_id; }
        const std::vector<Node *> &getCitations() const { return citations; }
    };

    Node * root;
};

#endif //JNP1_5_CITATION_GRAPH_H
