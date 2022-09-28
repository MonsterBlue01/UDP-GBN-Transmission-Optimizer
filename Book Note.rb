#!/usr/bin/ruby -w
# -*- coding : utf-8 -*-
 
print <<EOF
    这是第一种方式创建here document 。
    多行字符串。
EOF
 
print <<"EOF";                # 与上面相同
    这是第二种方式创建here document 。
    多行字符串。
EOF
 
print <<`EOC`                 # 执行命令
    echo hi there
    echo lo there
EOC
 
print <<"foo", <<"bar"          # 您可以把它们进行堆叠
    I said foo.
foo
    I said bar.
bar

# The output: 这是第一种方式创建here document 。
# 多行字符串。
# 这是第二种方式创建here document 。
# 多行字符串。
# hi there
# lo there
# I said foo.
# I said bar.

# Definition: 1.3.1:  Experiment and Event. An experiment is any process, real or hypothetical, in which
#                     the possible outcomes can be identified ahead of time. An event is a well-defined set
#                     of possible outcomes of the experiment.

#             1.4.1:  Sample Space. The collection of all possible outcomes of an experiment is called the
#                     sample space of the experiment.

#             1.4.2:  Containment. It is said that a set A is contained in another set B if every element
#                     of the set A also belongs to the set B. This relation between two events is expressed
#                     symbolically by the expression A ⊂ B, which is the set-theoretic expression for saying
#                     that A is a subset of B. Equivalently, if A ⊂ B, we may say that B contains A and may
#                     write B ⊃ A.

#             1.4.3:  Empty Set. The subset of S that contains no elements is called the empty set, or null set,
#                     and it is denoted by the symbol ∅.

#             1.4.4:  Countable/Uncountable. An infinite set A is countable if there is a one-to-one 
#                     correspondence between the elements of A and the set of natural numbers {1, 2, 3,...}. A
#                     set is uncountable if it is neither finite nor countable. If we say that a set has at most
#                     countably many elements, we mean that the set is either finite or countable.

#             1.4.5:  Complement. The complement of a set A is defined to be the set that contains all
#                     elements of the sample space S that do not belong to A. The notation for the
#                     complement of A is A^c.

#             1.4.6:  Union of Two Sets. If A and B are any two sets, the union of A and B is defined to be
#                     the set containing all outcomes that belong to A alone, to B alone, or to both A and
#                     B. The notation for the union of A and B is A ∪ B. The set A ∪ B is sketched in Fig. 
#                     1.2. In terms of events, A ∪ B is the event that either A or B or both occur. The union
#                     has the following properties whose proofs are left to the reader.

#             1.4.7:  Union of Many Sets. The union of n sets A1,...,An is defined to be the set that
#                     contains all outcomes that belong to at least one of these n sets. The notation for this
#                     union is either of the following: (See that on the book
                    
#             1.4.8:  Intersection of Two Sets. If A and B are any two sets, the intersection of A and B is
#                     defined to be the set that contains all outcomes that belong both to A and to B. The
#                     notation for the intersection of A and Bs.

#             1.4.9:  Intersection of Many Sets. The intersection of n sets A1,...,An is defined to be the
#                     set that contains the elements that are common to all these n sets. The notation for
#                     this intersection is A1 ∩ A2 ∩ ... ∩ An or ni=1 Ai. Similar notations are used for the
#                     intersection of an infinite sequence of sets or for the intersection of an arbitrary
#                     collection of sett A, B, and C be events. Then A ⊂ S. If A ⊂ B and B ⊂ A, then A = B. If A ⊂ B
#                     and B ⊂ C, then A ⊂ C

#             1.4.10: Disjoint/Mutually Exclusive. It is said that two sets A and B are disjoint, or mutually
#                     exclusive, if A and B have no outcomes in common, that is, if A ∩ B = ∅. The sets
#                     A1,...,An or the sets A1, A2,... are disjoint if for every i 	= j , we have that Ai and
#                     Aj are disjoint, that is, Ai ∩ Aj = ∅ for all i 	= j . The events in an arbitrary collection
#                     are disjoint if no two events in the collection have any outcomes in common.

# Theorem:    1.4.1:  Let A, B, and C be events. Then A ⊂ S. If A ⊂ B and B ⊂ A, then A = B. If A ⊂ B
#                     and B ⊂ C, then A ⊂ C.

#             1.4.2:  Let A be an event. Then ∅ ⊂ A.

#             1.4.3:  Let A be an event. Then (A^c) c = A, ∅^c = S, S^c = ∅. The empty event ∅ is an event.

#             1.4.4:  For all sets A and B, A ∪ B = B ∪ A, A ∪ A = A, A ∪ Ac = S, A ∪∅= A, A ∪ S = S.
#                     Furthermore, if A ⊂ B, then A ∪ B = B.

#             1.4.5:  The union of a finite number of events A1, ..., An is an event.

#             1.4.6:  Associative Property. For every three events A, B, and C, the following associative 
#                     relations are satisfied: A ∪ B ∪ C = (A ∪ B) ∪ C = A ∪ (B ∪ C).

#             1.4.7:  If A and B are events, then so is A ∩ B. For all events A and B,
#                     A ∩ B = B ∩ A, A ∩ A = A, A ∩ A^c = ∅, A ∩ ∅=∅, A ∩ S = A. 
#                     Furthermore, if A ⊂ B, then A ∩ B = A.

#             1.4.8:  Associative Property. For every three events A, B, and C, the following associative
#                     relations are satisfied: A ∩ B ∩ C = (A ∩ B) ∩ C = A ∩ (B ∩ C)

#             1.4.9:  De Morgan’s Laws. For every two sets A and B, (A ∪ B)^c = A^c ∩ B^c and (A ∩ B)^c = A^c ∪ B^c

#             1.4.10: Distributive Properties. For every three sets A, B, and C, A ∩ (B ∪ C) = (A ∩ B) ∪ (A ∩ C) and A ∪ (B ∩ C) = (A ∪ B) ∩ (A ∪ C).

#             1.4.11: Partitioning a Set. For every two sets A and B, A ∩ B and A ∩ B^c are disjoint and A = (A ∩ B) ∪ (A ∩ B^c).
#                     In addition, B and A ∩ B^c are disjoint, and A ∪ B = B ∪ (A ∩ B^c).

