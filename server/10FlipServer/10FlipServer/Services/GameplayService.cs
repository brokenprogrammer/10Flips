using _10FlipServer.Models;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace _10FlipServer.Services
{
    public class GameplayService
    {

        public User StartGame(Game game)
        {
            game.Started = true;
            game.Deck = new Deck();
            ResetDeck(game.Deck);

            User starter = null;
            Card lowest = null;

            foreach (User user in game.Users)
            {
                user.Hand = new List<Card>();

                user.BottomCards = new Card[3];
                user.TopCards = new Card[3];
                for (int i = 0; i < 3; i++)
                {
                    Card card1 = game.Deck.Cards.Pop();
                    user.BottomCards[i] = card1;
                    Card card2 = game.Deck.Cards.Pop();
                    user.TopCards[i] = card2;
                    Card card3 = game.Deck.Cards.Pop();
                    user.Hand.Add(card3);
                    if (lowest == null || card3.Value < lowest.Value || (card3.Value == lowest.Value && card3.Suit < lowest.Suit))
                    {
                        lowest = card3;
                        starter = user;
                    }
                }
            }

            game.PlacedCards = new Stack<Card>();
            game.CurrentUser = game.Users.IndexOf(starter);

            return starter;
        }

        public void HandlePlaceRequest(Game game, User user, CardType cardType)
        {
            int index = game.Users.IndexOf(user);
            if (index != game.CurrentUser)
            {
                return;
            }

            Card card = user.Hand.Where(c => c.Type == cardType).FirstOrDefault();
            if (card == null)
            {
                card = user.TopCards.Where(c => c.Type == cardType).FirstOrDefault();
            }
            if (card == null)
            {
                return;
            }

            if (CanUserPlace(game, user))
            {
                PlaceCard(game, user, card);
            }
        }

        /*
         * Returns true if user can place another card.
         */
        public bool PlaceCard(Game game, User user, Card card)
        {
            int index = game.Users.IndexOf(user);
            if (index != game.CurrentUser)
            {
                return false;
            }

            bool isHand = user.Hand.Contains(card);
            bool isTopCard = user.TopCards.Contains(card);

            if (isTopCard && user.Hand.Count > 0)
            {
                return false;
            }

            if (card.Value == 10)
            {
                // NOTE(Jesper): Reset placed cards.
                game.PlacedCards.Push(card);
                RemoveCardFromUser(card, user, game, isHand, isTopCard);
                game.PlacedCards = new Stack<Card>();
                return true;
            }
            else if (card.Value == 2)
            {
                // NOTE(Jesper): Allow same user to place another card.
                game.PlacedCards.Push(card);
                RemoveCardFromUser(card, user, game, isHand, isTopCard);
                return true;
            }
            else
            {
                Card topCard = game.PlacedCards.Count > 0 ? game.PlacedCards.Peek() : null;
                bool canPlace = topCard == null || card.Value >= topCard.Value;
                if (!canPlace)
                {
                    return false;
                }

                game.PlacedCards.Push(card);
                RemoveCardFromUser(card, user, game, isHand, isTopCard);

                return HasUserSameValue(game, user);
            }
        }

        public void RemoveCardFromUser(Card card, User user, Game game, bool isHand, bool isTopCard)
        {
            if (isHand)
            {
                user.Hand.Remove(card);
                if (game.Deck.Cards.Count > 0)
                {
                    user.Hand.Add(game.Deck.Cards.Pop());
                }
            }
            else if (isTopCard)
            {
                int i = Array.IndexOf(user.TopCards, card);
                if (user.BottomCards[i] != null)
                {
                    user.TopCards[i] = user.BottomCards[i];
                    user.BottomCards[i] = null;
                }
                else
                {
                    user.TopCards[i] = null;
                }
            }
        }

        public bool CanUserPlace(Game game, User user)
        {
            List<Card> cards = user.Hand;
            if (user.Hand.Count == 0)
            {
                cards = user.TopCards.ToList();
            }


            if (game.PlacedCards.Count == 0)
            {
                return true;
            }

            Card topCard = game.PlacedCards.Peek();
            return cards.Any(c => c.Value == 10 || c.Value == 2 || c.Value >= topCard.Value);
        }

        public bool HasUserSameValue(Game game, User user)
        {
            List<Card> cards = user.Hand;
            if (user.Hand.Count == 0)
            {
                cards = user.TopCards.ToList();
            }

            Card topCard = game.PlacedCards.Peek();
            return cards.Any(c => c.Value == topCard.Value);
        }

        public void ResetDeck(Deck deck)
        {
            List<Card> cards = new List<Card>();
            for (CardType type = CardType.TWO_OF_HEARTS; type <= CardType.ACE_OF_SPADES; type++)
            {
                cards.Add(new Card(type));
            }
            cards = cards.OrderBy(a => Guid.NewGuid()).ToList();

            deck.Cards = new Stack<Card>();
            for (int index = 0; index < cards.Count(); ++index)
            {
                Card card = cards[index];
                deck.Cards.Push(card);
            }
        }
    }
}
